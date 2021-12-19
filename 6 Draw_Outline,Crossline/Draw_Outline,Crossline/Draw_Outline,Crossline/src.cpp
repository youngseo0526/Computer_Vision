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
int GozalezBinThres(int* Histo) {
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

// 박스 평활화
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.11111, 0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

// 가우시안 평활화
void GaussAvgConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.0625, 0.125 ,0.0625,
                      0.125 ,0.25 ,0.125,
                      0.0625 ,0.125 ,0.0625 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

// Prewitt - X 마스크
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, 0.0 ,1.0,
                      -1.0 ,0.0 ,1.0,
                      -1.0 ,0.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 765 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

// Prewitt - Y 마스크
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      0.0 ,0.0 ,0.0,
                      1.0 ,1.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 1020 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

// Sobel - X 마스크
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, 0.0 ,1.0,
                      -2.0 ,0.0 ,2.0,
                      -1.0 ,0.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 1020 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

// Sobel - Y 마스크
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -2.0 ,-1.0,
                      0.0 ,0.0 ,0.0,
                      1.0 ,2.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 765 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

// Laplacian 마스크
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      -1.0, 8.0 ,-1.0,
                      -1.0 ,-1.0 ,-1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 세로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 2040 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
            Out[i * W + j] = abs((long)SumProduct) / 8;
            SumProduct = 0.0;
        }
    }
}

// 원본 밝기를 유지한 Laplacian 마스크
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      -1.0, 9.0 ,-1.0, // 영상 밝기 유지 위해 커널 합을 1로 설정 
                      -1.0 ,-1.0 ,-1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            //Out[i * W + j] = abs((long)SumProduct) / 8;
            if (SumProduct > 255.0)
                Out[i * W + j] = 255;
            else if (SumProduct < 0)
                Out[i * W + j] = 0;
            else
                Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
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

// 최대값 필터링(응용)
BYTE MaxPooling(BYTE* arr, int size) {

    // 오름차순 정렬
    const int S = size; // 리터럴 상수
    for (int i = 0; i < size - 1; i++) { // pivot index
        for (int j = i + 1; j < size; j++) { // 비교대상 index
            if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
        }
    }
    return arr[S - 1];
}

// 최소값 필터링(응용)
BYTE MinPooling(BYTE* arr, int size) {

    // 오름차순 정렬
    const int S = size; // 리터럴 상수
    for (int i = 0; i < size - 1; i++) { // pivot index
        for (int j = i + 1; j < size; j++) { // 비교대상 index
            if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
        }
    }
    return arr[0];
}

// 라벨링 함수에서 쓰이는 함수
int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top) {
    if (*top >= arr_size) return(-1);
    (*top)++;
    stackx[*top] = vx;
    stacky[*top] = vy;
    return(1);
}

// 라벨링 함수에서 쓰이는 함수
int pop(short* stackx, short* stacky, short* vx, short* vy, int* top) {
    if (*top == 0) return(-1);
    *vx = stackx[*top];
    *vy = stacky[*top];
    (*top)--;
    return(1);
}

// GlassFire 알고리즘을 이용한 라벨링 함수
void m_BlobColoring(BYTE* CutImage, int height, int width) {
    int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
    long k;
    short curColor = 0, r, c;
    //   BYTE** CutImage2;
    Out_Area = 1;


    // 스택으로 사용할 메모리 할당
    short* stackx = new short[height * width];
    short* stacky = new short[height * width];
    short* coloring = new short[height * width];

    int arr_size = height * width;

    // 라벨링된 픽셀을 저장하기 위해 메모리 할당
    for (k = 0; k < height * width; k++) coloring[k] = 0;  // 메모리 초기화
    for (i = 0; i < height; i++) {
        index = i * width;
        for (j = 0; j < width; j++) {
            // 이미 방문한 점이거나 픽셀값이 255가 아니라면 처리 안함
            if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
            r = i; c = j; top = 0; area = 1;
            curColor++;

            while (1) {
            GRASSFIRE:
                for (m = r - 1; m <= r + 1; m++) {
                    index = m * width;
                    for (n = c - 1; n <= c + 1; n++) {
                        //관심 픽셀이 영상경계를 벗어나면 처리 안함
                        if (m < 0 || m >= height || n < 0 || n >= width) continue;

                        if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0) {
                            coloring[index + n] = curColor; // 현재 라벨로 마크
                            if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
                            r = m; c = n; area++;
                            goto GRASSFIRE;
                        }
                    }
                }
                if (pop(stackx, stacky, &r, &c, &top) == -1) break;
            }
            if (curColor < 1000) BlobArea[curColor] = area;
        }
    }

    float grayGap = 255.0f / (float)curColor;

    // 가장 면적이 넓은 영역을 찾아내기 위함 
    for (i = 1; i <= curColor; i++) {
        if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
    }
    // CutImage 배열 클리어~
    for (k = 0; k < width * height; k++) CutImage[k] = 255;

    // coloring에 저장된 라벨링 결과중 (Out_Area에 저장된) 영역이 가장 큰 것만 CutImage에 저장
    for (k = 0; k < width * height; k++) {
        if (coloring[k] == Out_Area) CutImage[k] = 0;  // 가장 큰 것만 저장 (size filtering)
        //if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // 특정 면적 이상되는 영역만 출력
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap); // 원래 라벨링 결과 저장
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
} // 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

// 이진화 한다음 경계 검출
void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Bin[i * W + j] == 0) { // 전경 화소 라면
                if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && // 4방향 하소 중 하나라도 전경이 아니라면 
                    Bin[i * W + (j - 1)] == 0 && Bin[i * W + (j + 1)] == 0))
                    Out[i * W + j] = 255;
            }
        }
    }
}

// [과제 1)] 동공 영역에 외접하는 직사각형을 그리기
void DrawRectOutline(BYTE* Img, BYTE* Out, int W, int H, int* LU_X, int* LU_Y, int* RD_X, int* RD_Y) {
    // 1) 좌측상단부터 하방향 세로우선으로  탐색하다가, 0인화소를 처음 만났을 때의 X좌표를 저장 : x1
    for (int j = W - 1; j >= 0; j--) {
        for (int i = H - 1; i >= 0; i--) {
            if (Img[i * W + j] == 0)
                *LU_X = j;
        }
    }
    // 2) 우측하단부터 좌방향 가로우선으로 탐색하다가, 0인화소를 처음 만났을 때의 Y좌표를 저장 : y1
    for (int i = H - 1; i >= 0; i--) {
        for (int j = W - 1; j >= 0; j--) {
            if (Img[i * W + j] == 0)
                *LU_Y = i;
        }
    }
    // 3) 좌측상단부터 하방향 세로우선으로  탐색하다가, 0인화소를 처음 만났을 때의 X좌표를 저장 : x2
    for (int j = 0; j < W; j++) {
        for (int i = 0; i < H; i++) {
            if (Img[i * W + j] == 0)
                *RD_X = j;
        }
    }
    // 4) 우측하단부터 상방향 세로우선으로  탐색하다가, 0인화소를 처음 만났을 때의 X좌표를 저장 : y2
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Img[i * W + j] == 0)
                *RD_Y = i;
        }
    }
    //printf("%d  %d  %d  %d", *LU_X, *LU_Y, *RD_X, *RD_Y);

    // 좌표 사용하여 직사각형 그리기
    for (int i = 0; i < H; i++) { //y축
        for (int j = 0; j < W; j++) { //x축
            if ((j == *LU_X && i > *LU_Y && i < *RD_Y) || (i == *LU_Y && j > *LU_X && j < *RD_X) || (j == *RD_X && i > *LU_Y && i < *RD_Y) || (i == *RD_Y && j > *LU_X && j < *RD_X))
                Out[i * W + j] = 255;
        }
    }
}

// [과제 2)] 동공 영역의 무게중심을 통과하는 수평선/수직선 그리기
void DrawCrossLine(BYTE* Img, BYTE* Out, int W, int H, int* Cx, int* Cy) {
    // 동공 영역의 무게중심 찾기
    int count = 0;
    for (int i = 0; i < H; i++) {  // Y좌표
        for (int j = 0; j < W; j++) { // X좌표
            if (Img[i * W + j] == 0) {
                *Cx += j;
                *Cy += i;
                count++;
            }
        }
    }
    *Cx /= count;
    *Cy /= count;

    // 무게중심 기준 수평선/수직선 그리기
    for (int i = 0; i < H; i++) { // Y축
        for (int j = 0; j < W; j++) { // X축
            if (j == *Cx || i == *Cy)
                Out[i * W + j] = 255;
        }
    }
}

int main()
{
    /* 영상 입력 */
    BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
    BITMAPINFOHEADER hInfo; // BMP 인포헤더 40Bytes
    RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes = 1024Bytes)
    FILE* fp; // 파일 포인터 : 데이터를 읽거나 내보낼 때 필요
    fp = fopen("pupil2.bmp", "rb"); // 제일 첫번째 주소나 위치를 가리킴 (read binary)
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
    BYTE* Temp = (BYTE*)malloc(ImgSize); // 임시배열
    BYTE* Temp2 = (BYTE*)malloc(ImgSize);
    BYTE* Output = (BYTE*)malloc(ImgSize); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일) 
    BYTE* Output2 = (BYTE*)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
    fclose(fp); // 파일포인터와 파일 간의 관계 끊기

    int H = hInfo.biHeight, W = hInfo.biWidth;
    int Histo[256] = { 0, }; // 히스토그램
    int AHisto[256] = { 0, }; // 누적히스토그램

    Binarization(Image, Temp, W, H, 50);
    InverseImage(Temp, Temp, W, H); // 전경이 어두운 부분으로 출력되도록 영상반전
    m_BlobColoring(Temp, H, W);
    for (int i = 0; i < ImgSize; i++) Output[i] = Image[i];
    //for (int i = 0; i < ImgSize; i++) Output2[i] = Image[i];
    //BinaryImageEdgeDetection(Temp, Output, W, H);

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0; // 좌상, 우하의 xy좌표
    DrawRectOutline(Temp, Output, W, H, &x1, &y1, &x2, &y2); // [과제 1)]

    int X = 0, Y = 0; // 교차점의 X, Y 좌표 
    DrawCrossLine(Temp, Output, W, H, &X, &Y); // [과제 2)]

    /* Median filtering*/
    //BYTE temp[9];
    //int W = hInfo.biWidth, H = hInfo.biHeight;
    //int i, j;
    //for (i = 1; i < H - 1; i++) {
    //   for (j = 1; j < W - 1; j++) {
    //      temp[0] = Image[(i - 1) * W + j-1];
    //      temp[1] = Image[(i - 1) * W + j];
    //      temp[2] = Image[(i - 1) * W + j+1];
    //      temp[3] = Image[i * W + j-1];
    //      temp[4] = Image[i * W + j]; // center position
    //      temp[5] = Image[i * W + j+1];
    //      temp[6] = Image[(i + 1) * W + j-1];
    //      temp[7] = Image[(i + 1)* W + j];
    //      temp[8] = Image[(i + 1)* W + j+1];
    //      //Output[i * W + j] = Median(temp, 9);
    //      //Output[i * W + j] = MaxPooling(temp, 9);
    //      //Output[i * W + j] = MinPooling(temp, 9);
    //   }
    //}
    /* Median filtering*/

    /* Median filtering */
    //int Length = 5;  // 마스크의 한 변의 길이 (홀수)
    //int Margin = Length / 2; // 영상의 필터링 처리 시 오류 방지를 위한 마진값
    //int WSize = Length * Length; // 필터(마스크) 윈도우 크기
    //BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // BYTE형으로 마스크 크기의 메모리 할당
    //int W = hInfo.biWidth, H = hInfo.biHeight; // 영상의 가로, 세로
    //int i, j, m, n;  // 배열의 인덱스 접근을 위한 변수
    //for (i = Margin; i < H - Margin; i++) { // 상,하 margin을 고려한 마스크의 Y좌표(행)
    //   for (j = Margin; j < W - Margin; j++) { // 좌,우 margin을 고려한 마스크의 X좌표(열)
    //      for (m = -Margin; m <= Margin; m++) { // i와 연결되어 마스크 중심 기준 세로방향 이동(행)
    //         for (n = -Margin; n <= Margin; n++) { // j와 연결되어 마스크 중심 기준 가로방향 이동(열)
    //            temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n]; //
    //         }
    //      }
    //      Output[i * W + j] = Median(temp, WSize); // 중간 값을 Output배열의 i행 j열에 저장
    //   }
    //}
    //free(temp); // 동적할당 메모리 해제
    /* Median filtering */


    //ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);

    /* 영상처리 */
    //AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvgConv(Image, Output, hInfo.biWidth, hInfo.biHeight);

    //Prewitt_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
    //Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //for (int i = 0; i < ImgSize; i++) { // Prewitt_X_Conv로 컨볼루션 한 후 Prewitt_Y_Conv으로 컨볼루션하여 영상 출력
    //   if (Temp[i] > Output[i]) Output[i] = Temp[i];
    //}
    //Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40); // Prewitt_Conv로 추출한 경계값을 이진화하여 출력

    //Sobel_X_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //for (int i = 0; i < ImgSize; i++) { // Sobel_X_Conv로 컨볼루션 한 후 Sobel_Y_Conv으로 컨볼루션하여 영상 출력
    //   if (Temp[i] > Output[i]) Output[i] = Temp[i];
    //}
    //Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40); // Sobel_Conv로 추출한 경계값을 이진화하여 출력

    //Laplace_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvgConv(Image, Temp, hInfo.biWidth, hInfo.biHeight); // Laplace_Conv_DC만 돌리면 영상의 노이즈가 증폭되기 때문에 가우시안 평활화로 먼저 제거해줌
    //Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight); // 원본 밝기 유지하면서 경계, 노이즈를 증폭시킴

    //ObtainAHistogram(Histo, AHisto);
    //HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
    //HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);

    //int Thres = GozalezBinThres(Histo);

    //Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
    //printf("Threshold by Gonzalez : %d", Thres);
    //InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
    //ContrastAdj(Image, Output , hInfo.biWidth, hInfo.biHeight, 0.5);


    /* 처리된 결과 출력 */
    SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_pupil2.bmp");

    free(Image); // 동적할당 메모리 해제
    free(Output); // ..
    free(Temp);
    return 0;
}