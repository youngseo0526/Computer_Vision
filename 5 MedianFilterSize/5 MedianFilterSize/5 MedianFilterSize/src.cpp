#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // 헤더의 구조체 정보 포함

// swap 함수
void swap(BYTE* a, BYTE* b) {
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

// 중간값 필터링
BYTE Median(BYTE* arr, int size) {

	// 오름차순 정렬
	const int S = size; // 리터럴 상수
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // 비교대상 index
			if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

// 처리된 결과 출력
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName) {
	FILE* fp = fopen(FileName, "wb"); // 비어있는 제일 첫번째 주소나 위치 가리킴 (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf변수의 정보를 파일에 출력
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), W * H, fp); // 영상의 화소 정보 출력(새롭게 저장된 Output을 출력)
	fclose(fp);
}

int main()
{
	/* 영상 입력 */
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes = 1024Bytes)
	FILE* fp; // 파일 포인터 : 데이터를 읽거나 내보낼 때 필요
	fp = fopen("lenna_impulse.bmp", "rb"); // 제일 첫번째 주소나 위치를 가리킴 (read binary)
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
	BYTE* Temp = (BYTE*)malloc(ImgSize); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일)
	BYTE* Output = (BYTE*)malloc(ImgSize); // 임시배열
	fread(Image, sizeof(BYTE), ImgSize, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
	fclose(fp); // 파일포인터와 파일 간의 관계 끊기



/* Median filtering */
	int Length = 5;  // 마스크의 한 변의 길이 (홀수)
	int Margin = Length / 2; // 영상의 필터링 처리 시 오류 방지를 위한 마진값
	int WSize = Length * Length; // 필터(마스크) 윈도우 크기
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // BYTE형으로 마스크 크기의 메모리 할당
	int W = hInfo.biWidth, H = hInfo.biHeight; // 영상의 가로, 세로
	int i, j, m, n;  // 배열의 인덱스 접근을 위한 변수

	for (i = Margin; i < H - Margin; i++) { // 상,하 margin을 고려한 마스크의 Y좌표(행)
		for (j = Margin; j < W - Margin; j++) { // 좌,우 margin을 고려한 마스크의 X좌표(열)
			for (m = -Margin; m <= Margin; m++) { // i와 연결되어 마스크 중심 기준 세로방향 이동(행)
				for (n = -Margin; n <= Margin; n++) { // j와 연결되어 마스크 중심 기준 가로방향 이동(열)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n]; //
				}
			}
			Output[i * W + j] = Median(temp, WSize); // 중간 값을 Output배열에 저장
		}
	}
	free(temp); // 동적할당 메모리 해제
	/* Median filtering */



	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_9.bmp");

	free(Image); // 동적할당 메모리 해제
	free(Output); // ..
	free(Temp);
	return 0;
}