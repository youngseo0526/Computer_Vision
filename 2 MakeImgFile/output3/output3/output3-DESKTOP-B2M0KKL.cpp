/* 원본 영상의 반전 결과 영상(negative image) */

#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>  // 헤더의 구조체 정보 포함
void main()
{
	/* 영상 입력 */
	BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
	BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
	RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes = 1024Bytes)
	FILE *fp; // 파일 포인터 : 데이터를 읽거나 내보낼 때 필요
	fp = fopen("lenna.bmp", "rb"); // 제일 첫번째 주소나 위치를 가리킴 (read binary)
	if (fp == NULL) return; // 예외 검사

	// fread함수로 정보를 읽어 메모리 변수에 담는 과정 (포인터는 함수 처리후 다음 곳을 가리킴)
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp에서 부터 파일헤더의 크기만큼(14) 읽어 hf에 저장
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp로 부터 인포헤더의 크기만큼(40) 읽어 hinfo에 저장
	fread(hRGB, sizeof(RGBQUAD), 256, fp); // fp로 부터 팔레트의 크기만큼(1024) 읽어 hRGB에 저장

	// 영상의 화소 정보 읽기 (동적할당) , *malloc 함수 : 동적 할당한 메모리의 첫번째 주소 반환
	int ImgSize = hInfo.biWidth * hInfo.biHeight; // 영상의 가로 * 세로 사이즈 => 영상 전체 픽셀의 개수(영상 전체의 크기)
	BYTE * Image = (BYTE *)malloc(ImgSize); // Image 배열 : 원래 영상의 화소 정보를 담을 공간
	BYTE * Output = (BYTE *)malloc(ImgSize); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일)
	fread(Image, sizeof(BYTE), ImgSize, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
	fclose(fp); // 파일포인터와 파일 간의 관계 끊기

	/* 처리된 결과 출력 */
	fp = fopen("output3.bmp", "wb"); // 비어있는 제일 첫번째 주소나 위치 가리킴 (write binary)
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf변수의 정보를 파일에 출력
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
	fwrite(Output, sizeof(BYTE), ImgSize, fp); // 영상의 화소 정보 출력(새롭게 저장된 Output을 출력)
	fclose(fp);
	free(Image); // 동적할당 메모리 해제
	free(Output); // ..
}