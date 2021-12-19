#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // 헤더의 구조체 정보 포함

// 영상반전
void InverseImage(BYTE* Img, BYTE* Out, int W, int H) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

// 밝기조절 
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] + Val > 255)
			Out[i] = 255;
		else if (Img[i] + Val < 0)
			Out[i] = 0;
		else Out[i] = Img[i] + Val;
	}
}

// 대비조절
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] * Val > 255.0)
			Out[i] = 255;
		else Out[i] = (BYTE)(Img[i] * Val); // 강제 형 변환
	}
}

// 히스토그램 구하기
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {

	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) { //히스토그램 구하기
		Histo[Img[i]]++; // count
	}
}

// 누적히스토그램 구하기
void ObtainAHistogram(int* Histo, int* AHisto) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

// 스트레칭
void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H) {
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)(((Img[i] - Low) / (double)(High - Low)) * 255.0);
	}
}

// 평활화
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];  // 정규화합(double형이 아닌 이유:어짜피 평활화에 사용되는 정규화합은 정수형으로 변환되어 소수자리를 날리기 때문에 c언어 기능을 사용하여 자동으로 소수점을 버린 것)
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++) { // 모든 영상의 화소값을 정규화합으로 바꾸기
		Out[i] = NormSum[Img[i]];
	}
}

// 이진화
void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

// 영상 밝기의 최대값 구하기
int LowBrightness(int* arr) {
	int Low = 0;
	for (int i = 0; i < 256; i++) {
		if (arr[i] != 0) {
			Low = i;
			break;
		}
	}
	return Low;
}

// 영상 밝기의 최소값 구하기
int HightBrightness(int* arr) {
	int High = 255;
	for (int i = 255; i >= 0; i--) {
		if (arr[i] != 0) {
			High = i;
			break;
		}
	}
	return High;
}

// Gonzalez 경계값 자동 결정
int GozalezBinThres(int*Histo) {
	int Low = LowBrightness(Histo);
	int High = HightBrightness(Histo);
	double t_old = (Low + High) / 2; // 초기 경계값 (93)
	double t_new = 0;
	double SUM1 = 0; // (계급값)x(도수)의 총합
	double SUM2 = 0;
	double sum1 = 0; // 도수의 총합
	double sum2 = 0;
	double m1, m2;// 화소들의 밝기의 평균값

	while (1) {
		int g1[256] = { 0, }; // 밝기값이 t보다 큰 화소로 구성된 영역
		int g2[256] = { 0, }; // 밝기값이 t보다 작은 화소로 구성된 영역

		// t를 경계로 영역 나누기
		for (int i = 0; i < 256; i++) {
			if (i < t_old)
				g1[i] = Histo[i];
			else
				g2[i] = Histo[i];
		}

		// 영역들의 평균 구하기
		for (int i = 0; i < 256; i++) {
			if (g1[i] != 0) {
				SUM1 += i * g1[i];
				sum1 += g1[i];
				m1 = (double)(SUM1 / sum1);
			}
			if (g2[i] != 0) {
				SUM2 += i * g2[i];
				sum2 += g2[i];
				m2 = (double)(SUM2 / sum2);
			}
		}

		t_new = (m1 + m2) / 2;
		if (fabs(t_old - t_new) < 3)
			break;
		else
			t_old = t_new;
	}
	return t_new;
}

int main()
{
	/* 영상 입력 */
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes = 1024Bytes)
	FILE* fp; // 파일 포인터 : 데이터를 읽거나 내보낼 때 
	fp = fopen("coin.bmp", "rb"); // 제일 첫번째 주소나 위치를 가리킴 (read binary)
	if (fp == NULL) { // 예외 검사
		printf("File not found!\n");
		return -1;
	}

	// fread함수로 정보를 읽어 메모리 변수에 담는 과정 (포인터는 함수 처리후 다음 곳을 가리킴)
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp에서 부터 파일헤더의 크기만큼(14) 읽어 hf에 저장
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp로 부터 인포헤더의 크기만큼(40) 읽어 hinfo에 저장
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // fp로 부터 팔레트의 크기만큼(1024) 읽어 hRGB에 저장

	// 영상의 화소 정보 읽기 (동적할당) , *malloc 함수 : 동적 할당한 메모리의 첫번째 주소 반환
	int ImgSize = hInfo.biWidth * hInfo.biHeight; // 영상의 가로 * 세로 사이즈 => 영상 전체 픽셀의 개수(영상 전체의 크기)
	BYTE* Image = (BYTE*)malloc(ImgSize); // Image 배열 : 원래 영상의 화소 정보를 담을 공간
	BYTE* Output = (BYTE*)malloc(ImgSize); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일)
	fread(Image, sizeof(BYTE), ImgSize, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
	fclose(fp); // 파일포인터와 파일 간의 관계 끊기

	int Histo[256] = { 0, }; // 히스토그램
	int AHisto[256] = { 0, }; // 누적히스토그램

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);

	/* 영상처리 */
	//ObtainAHistogram(Histo, AHisto);
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);

	int Thres = GozalezBinThres(Histo);

	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	printf("Threshold by Gonzalez : %d", Thres);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output , hInfo.biWidth, hInfo.biHeight, 0.5);

	/* 처리된 결과 출력 */
	fp = fopen("BinarizationByGozalez_output.bmp", "wb"); // 비어있는 제일 첫번째 주소나 위치 가리킴 (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf변수의 정보를 파일에 출력
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // 영상의 화소 정보 출력(새롭게 저장된 Output을 출력)
	fclose(fp);
	free(Image); // 동적할당 메모리 해제
	free(Output); // ..
	return 0;
}