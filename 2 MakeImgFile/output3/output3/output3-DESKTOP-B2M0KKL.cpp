/* ���� ������ ���� ��� ����(negative image) */

#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>  // ����� ����ü ���� ����
void main()
{
	/* ���� �Է� */
	BITMAPFILEHEADER hf; // BMP ������� 14Bytes
	BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
	RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes = 1024Bytes)
	FILE *fp; // ���� ������ : �����͸� �аų� ������ �� �ʿ�
	fp = fopen("lenna.bmp", "rb"); // ���� ù��° �ּҳ� ��ġ�� ����Ŵ (read binary)
	if (fp == NULL) return; // ���� �˻�

	// fread�Լ��� ������ �о� �޸� ������ ��� ���� (�����ʹ� �Լ� ó���� ���� ���� ����Ŵ)
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp���� ���� ��������� ũ�⸸ŭ(14) �о� hf�� ����
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp�� ���� ��������� ũ�⸸ŭ(40) �о� hinfo�� ����
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // fp�� ���� �ȷ�Ʈ�� ũ�⸸ŭ(1024) �о� hRGB�� ����

	// ������ ȭ�� ���� �б� (�����Ҵ�) , *malloc �Լ� : ���� �Ҵ��� �޸��� ù��° �ּ� ��ȯ
	int ImgSize = hInfo.biWidth * hInfo.biHeight; // ������ ���� * ���� ������ => ���� ��ü �ȼ��� ����(���� ��ü�� ũ��)
	BYTE * Image = (BYTE *)malloc(ImgSize); // Image �迭 : ���� ������ ȭ�� ������ ���� ����
	BYTE * Output = (BYTE *)malloc(ImgSize); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����)
	fread(Image, sizeof(BYTE), ImgSize, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
	fclose(fp); // ���������Ϳ� ���� ���� ���� ����

	/* ó���� ��� ��� */
	fp = fopen("output3.bmp", "wb"); // ����ִ� ���� ù��° �ּҳ� ��ġ ����Ŵ (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
	fclose(fp);
	free(Image); // �����Ҵ� �޸� ����
	free(Output); // ..
}