/*  ���� ������ ��Ⱚ�� 50��ŭ ������Ų ���� (���� overflow�� ���� Ŭ����ó���� �ʿ����) */

#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int main()
{
	BITMAPFILEHEADER hf; //14����Ʈ
	BITMAPINFOHEADER hInfo; //40����Ʈ
	RGBQUAD hRGB[256]; //1024����Ʈ
	FILE *fp;
	fp = fopen("lenna.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(&hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE*Image = (BYTE*)malloc(ImgSize);
	BYTE*Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	for (int i = 0; i < ImgSize; i++)
		Output[i] = Image[i] + 50;

	fp = fopen("output2.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}