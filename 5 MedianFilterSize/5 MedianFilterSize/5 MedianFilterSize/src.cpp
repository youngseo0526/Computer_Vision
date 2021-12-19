#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // ����� ����ü ���� ����

// swap �Լ�
void swap(BYTE* a, BYTE* b) {
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

// �߰��� ���͸�
BYTE Median(BYTE* arr, int size) {

	// �������� ����
	const int S = size; // ���ͷ� ���
	for (int i = 0; i < size - 1; i++) { // pivot index
		for (int j = i + 1; j < size; j++) { // �񱳴�� index
			if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
		}
	}
	return arr[S / 2];
}

// ó���� ��� ���
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName) {
	FILE* fp = fopen(FileName, "wb"); // ����ִ� ���� ù��° �ּҳ� ��ġ ����Ŵ (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), W * H, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
	fclose(fp);
}

int main()
{
	/* ���� �Է� */
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes = 1024Bytes)
	FILE* fp; // ���� ������ : �����͸� �аų� ������ �� �ʿ�
	fp = fopen("lenna_impulse.bmp", "rb"); // ���� ù��° �ּҳ� ��ġ�� ����Ŵ (read binary)
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
	BYTE* Temp = (BYTE*)malloc(ImgSize); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����)
	BYTE* Output = (BYTE*)malloc(ImgSize); // �ӽù迭
	fread(Image, sizeof(BYTE), ImgSize, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
	fclose(fp); // ���������Ϳ� ���� ���� ���� ����



/* Median filtering */
	int Length = 5;  // ����ũ�� �� ���� ���� (Ȧ��)
	int Margin = Length / 2; // ������ ���͸� ó�� �� ���� ������ ���� ������
	int WSize = Length * Length; // ����(����ũ) ������ ũ��
	BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // BYTE������ ����ũ ũ���� �޸� �Ҵ�
	int W = hInfo.biWidth, H = hInfo.biHeight; // ������ ����, ����
	int i, j, m, n;  // �迭�� �ε��� ������ ���� ����

	for (i = Margin; i < H - Margin; i++) { // ��,�� margin�� ����� ����ũ�� Y��ǥ(��)
		for (j = Margin; j < W - Margin; j++) { // ��,�� margin�� ����� ����ũ�� X��ǥ(��)
			for (m = -Margin; m <= Margin; m++) { // i�� ����Ǿ� ����ũ �߽� ���� ���ι��� �̵�(��)
				for (n = -Margin; n <= Margin; n++) { // j�� ����Ǿ� ����ũ �߽� ���� ���ι��� �̵�(��)
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n]; //
				}
			}
			Output[i * W + j] = Median(temp, WSize); // �߰� ���� Output�迭�� ����
		}
	}
	free(temp); // �����Ҵ� �޸� ����
	/* Median filtering */



	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "median_9.bmp");

	free(Image); // �����Ҵ� �޸� ����
	free(Output); // ..
	free(Temp);
	return 0;
}