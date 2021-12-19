#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // ����� ����ü ���� ����

// �������
void InverseImage(BYTE* Img, BYTE* Out, int W, int H) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = 255 - Img[i];
	}
}

// ������� 
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

// �������
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] * Val > 255.0)
			Out[i] = 255;
		else Out[i] = (BYTE)(Img[i] * Val); // ���� �� ��ȯ
	}
}

// ������׷� ���ϱ�
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {

	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) { //������׷� ���ϱ�
		Histo[Img[i]]++; // count
	}
}

// ����������׷� ���ϱ�
void ObtainAHistogram(int* Histo, int* AHisto) {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] += Histo[j];
		}
	}
}

// ��Ʈ��Ī
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

// ��Ȱȭ
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H) {
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];  // ����ȭ��(double���� �ƴ� ����:��¥�� ��Ȱȭ�� ���Ǵ� ����ȭ���� ���������� ��ȯ�Ǿ� �Ҽ��ڸ��� ������ ������ c��� ����� ����Ͽ� �ڵ����� �Ҽ����� ���� ��)
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	for (int i = 0; i < ImgSize; i++) { // ��� ������ ȭ�Ұ��� ����ȭ������ �ٲٱ�
		Out[i] = NormSum[Img[i]];
	}
}

// ����ȭ
void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold) {
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

// ���� ����� �ִ밪 ���ϱ�
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

// ���� ����� �ּҰ� ���ϱ�
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

// Gonzalez ��谪 �ڵ� ����
int GozalezBinThres(int*Histo) {
	int Low = LowBrightness(Histo);
	int High = HightBrightness(Histo);
	double t_old = (Low + High) / 2; // �ʱ� ��谪 (93)
	double t_new = 0;
	double SUM1 = 0; // (��ް�)x(����)�� ����
	double SUM2 = 0;
	double sum1 = 0; // ������ ����
	double sum2 = 0;
	double m1, m2;// ȭ�ҵ��� ����� ��հ�

	while (1) {
		int g1[256] = { 0, }; // ��Ⱚ�� t���� ū ȭ�ҷ� ������ ����
		int g2[256] = { 0, }; // ��Ⱚ�� t���� ���� ȭ�ҷ� ������ ����

		// t�� ���� ���� ������
		for (int i = 0; i < 256; i++) {
			if (i < t_old)
				g1[i] = Histo[i];
			else
				g2[i] = Histo[i];
		}

		// �������� ��� ���ϱ�
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
	/* ���� �Է� */
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes = 1024Bytes)
	FILE* fp; // ���� ������ : �����͸� �аų� ������ �� 
	fp = fopen("coin.bmp", "rb"); // ���� ù��° �ּҳ� ��ġ�� ����Ŵ (read binary)
	if (fp == NULL) { // ���� �˻�
		printf("File not found!\n");
		return -1;
	}

	// fread�Լ��� ������ �о� �޸� ������ ��� ���� (�����ʹ� �Լ� ó���� ���� ���� ����Ŵ)
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp���� ���� ��������� ũ�⸸ŭ(14) �о� hf�� ����
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp�� ���� ��������� ũ�⸸ŭ(40) �о� hinfo�� ����
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // fp�� ���� �ȷ�Ʈ�� ũ�⸸ŭ(1024) �о� hRGB�� ����

	// ������ ȭ�� ���� �б� (�����Ҵ�) , *malloc �Լ� : ���� �Ҵ��� �޸��� ù��° �ּ� ��ȯ
	int ImgSize = hInfo.biWidth * hInfo.biHeight; // ������ ���� * ���� ������ => ���� ��ü �ȼ��� ����(���� ��ü�� ũ��)
	BYTE* Image = (BYTE*)malloc(ImgSize); // Image �迭 : ���� ������ ȭ�� ������ ���� ����
	BYTE* Output = (BYTE*)malloc(ImgSize); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����)
	fread(Image, sizeof(BYTE), ImgSize, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
	fclose(fp); // ���������Ϳ� ���� ���� ���� ����

	int Histo[256] = { 0, }; // ������׷�
	int AHisto[256] = { 0, }; // ����������׷�

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);

	/* ����ó�� */
	//ObtainAHistogram(Histo, AHisto);
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);

	int Thres = GozalezBinThres(Histo);

	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
	printf("Threshold by Gonzalez : %d", Thres);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
	//ContrastAdj(Image, Output , hInfo.biWidth, hInfo.biHeight, 0.5);

	/* ó���� ��� ��� */
	fp = fopen("BinarizationByGozalez_output.bmp", "wb"); // ����ִ� ���� ù��° �ּҳ� ��ġ ����Ŵ (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
	fclose(fp);
	free(Image); // �����Ҵ� �޸� ����
	free(Output); // ..
	return 0;
}