#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // 헤더의 구조체 정보 포함

// 2차원 배열 동적할당 위함
unsigned char** imageMatrix;
// 이진영상에서 
unsigned char blankPixel = 255, imagePixel = 0;

typedef struct {
    int row, col;
}pixel;

int getBlackNeighbours(int row, int col) {
    int i, j, sum = 0;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (i != 0 || j != 0)
                sum += (imageMatrix[row + i][col + j] == imagePixel);
        }
    }
    return sum;
}

int getBWTransitions(int row, int col) {
    return 	((imageMatrix[row - 1][col] == blankPixel && imageMatrix[row - 1][col + 1] == imagePixel)
        + (imageMatrix[row - 1][col + 1] == blankPixel && imageMatrix[row][col + 1] == imagePixel)
        + (imageMatrix[row][col + 1] == blankPixel && imageMatrix[row + 1][col + 1] == imagePixel)
        + (imageMatrix[row + 1][col + 1] == blankPixel && imageMatrix[row + 1][col] == imagePixel)
        + (imageMatrix[row + 1][col] == blankPixel && imageMatrix[row + 1][col - 1] == imagePixel)
        + (imageMatrix[row + 1][col - 1] == blankPixel && imageMatrix[row][col - 1] == imagePixel)
        + (imageMatrix[row][col - 1] == blankPixel && imageMatrix[row - 1][col - 1] == imagePixel)
        + (imageMatrix[row - 1][col - 1] == blankPixel && imageMatrix[row - 1][col] == imagePixel));
}

int zhangSuenTest1(int row, int col) {
    int neighbours = getBlackNeighbours(row, col);
    return ((neighbours >= 2 && neighbours <= 6)
        && (getBWTransitions(row, col) == 1)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel)
        && (imageMatrix[row][col + 1] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col - 1] == blankPixel));
}

int zhangSuenTest2(int row, int col) {
    int neighbours = getBlackNeighbours(row, col);
    return ((neighbours >= 2 && neighbours <= 6)
        && (getBWTransitions(row, col) == 1)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel || imageMatrix[row][col - 1] == blankPixel)
        && (imageMatrix[row - 1][col] == blankPixel || imageMatrix[row + 1][col] == blankPixel || imageMatrix[row][col + 1] == blankPixel));
}

void zhangSuen(unsigned char* image, unsigned char* output, int rows, int cols) {
    int startRow = 1, startCol = 1, endRow, endCol, i, j, count, processed;
    pixel* markers;
    imageMatrix = (unsigned char**)malloc(rows * sizeof(unsigned char*));
    for (i = 0; i < rows; i++) {
        imageMatrix[i] = (unsigned char*)malloc((cols + 1) * sizeof(unsigned char));
        for (int k = 0; k < cols; k++) imageMatrix[i][k] = image[i * cols + k];
    }
    endRow = rows - 2;
    endCol = cols - 2;
    do {
        markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
        count = 0;
        for (i = startRow; i <= endRow; i++) {
            for (j = startCol; j <= endCol; j++) {
                if (imageMatrix[i][j] == imagePixel && zhangSuenTest1(i, j) == 1) {
                    markers[count].row = i;
                    markers[count].col = j;
                    count++;
                }
            }
        }
        processed = (count > 0);
        for (i = 0; i < count; i++) {
            imageMatrix[markers[i].row][markers[i].col] = blankPixel;
        }
        free(markers);
        markers = (pixel*)malloc((endRow - startRow + 1) * (endCol - startCol + 1) * sizeof(pixel));
        count = 0;
        for (i = startRow; i <= endRow; i++) {
            for (j = startCol; j <= endCol; j++) {
                if (imageMatrix[i][j] == imagePixel && zhangSuenTest2(i, j) == 1) {
                    markers[count].row = i;
                    markers[count].col = j;
                    count++;
                }
            }
        }
        if (processed == 0)
            processed = (count > 0);
        for (i = 0; i < count; i++) {
            imageMatrix[markers[i].row][markers[i].col] = blankPixel;
        }
        free(markers);
    } while (processed == 1);
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            output[i * cols + j] = imageMatrix[i][j];
        }
    }
}

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
        else Out[i] = (BYTE)(Img[i] * Val);
    }
}

// 히스토그램 구하기
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++) {
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
    for (int i = 0; i < ImgSize; i++) { // 모든 영상의 화소값을 정규화합으로 바꾸기(매핑)
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

// 권장 답안 //
BYTE DetermThGonzalez(int* H) {
    BYTE ep = 3;
    BYTE Low, High;
    BYTE Th, NewTh;
    int G1 = 0, G2 = 0, cnt1 = 0, cnt2 = 0, mu1, mu2;

    for (int i = 0; i < 256; i++) {
        if (H[i] != 0)
            Low = i;
        break;
    }
    for (int i = 255; i >= 0; i--) {
        if (H[i] != 0)
            High = i;
        break;
    }
    if (cnt1 == 0) cnt1 = 1;
    if (cnt2 == 0) cnt2 = 1;
    Th = (Low + High) / 2;
    while (1) {
        for (int i = Th; i <= High; i++) {
            G1 += (H[i] * i);
            cnt1 += H[i];
        }
        for (int i = Low; i <= Th; i++) {
            G2 += (H[i] * i);
            cnt2 += H[i];
        }
        mu1 = G1 / cnt1;
        mu2 = G2 / cnt2;

        NewTh = (mu1 + mu1) / 2;
        if (abs(NewTh - Th) < ep) {
            Th = NewTh;
            break;
        }
        else Th = NewTh;
        G1 = G2 = cnt1 = cnt2 = 0;
    }
    return Th;
}

// 박스 평활화(평균)
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.11111, 0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

// 가우시안 평활화(박스 평활화보다 원본값 더 유지)
void GaussAvgConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.0625, 0.125 ,0.0625,
                      0.125 ,0.25 ,0.125,
                      0.0625 ,0.125 ,0.0625 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner의 Y좌표(행) (마진을 남겨 놓기 위해 1부터 시작, -1에서 종료)
        for (int j = 1; j < W - 1; j++) { // Kernel Center의 X좌표(열)
            for (int m = -1; m <= 1; m++) { // Kernel 중심 기준 세로방향 이동(행)
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
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
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 765 ==> 0 ~ 255로 정규화
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
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 765 ==> 0 ~ 255로 정규화
            Out[i * W + j] = abs((long)SumProduct) / 3;
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
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 1024 ===> 0 ~ 255로 정규화
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
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 1024 ===> 0 ~ 255로 정규화
            Out[i * W + j] = abs((long)SumProduct) / 4;
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
                for (int n = -1; n <= 1; n++) { // Kernel 중심 기준 가로방향 이동(열)
                   /* i*W + j : 2차원배열에서 1차원적으로 인덱싱 하는 법 */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // 인덱스가 음수일 수 없기 때문에 +1해줌
                }
            }
            // 0 ~ 2048 ===> 0 ~ 255로 정규화 (마스크의 -1을 곱한 값의 합이 1을 곱한 값의 합보다 클 경우 255 이상 나올 수 있음)
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
    if (hInfo.biBitCount == 24) {
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf변수의 정보를 파일에 출력
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
        fwrite(Output, sizeof(BYTE), W * H * 3, fp); // 영상의 화소 정보 출력(새롭게 저장된 Output을 출력)
    }
    else if (hInfo.biBitCount == 8) {
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf변수의 정보를 파일에 출력
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
        fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
        fwrite(Output, sizeof(BYTE), W * H, fp); // 영상의 화소 정보 출력(새롭게 저장된 Output을 출력)
    }
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
    short curColor = 0, r, c; // curColor: 총 component 개수
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
            if (curColor < 1000) BlobArea[curColor] = area; // labeling된 area 배열
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
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap); // 원래 라벨링 결과 저장 (255를 라벨링된 영역개수(25개)로 나눠 저장)
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
}
// 라벨링 후 가장 넓은 영역에 대해서만 뽑아내는 코드 포함

// 이진화 한다음 경계 검출
void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Bin[i * W + j] == 0) { // 전경 화소 라면
                if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && // 전경 화소 중 주변 4방향 화소가 하나라도 전경이 아니면 경계
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

// 위,아래 변환
void VerticalFilp(BYTE* Img, int W, int H) {
    for (int i = 0; i < H / 2; i++) {  // y좌표
        for (int j = 0; j < W; j++)  // x좌표
            swap(&Img[i * W + j], &Img[(H - 1 - i) * W + j]);
    }
}

// 좌, 우 변환
void HorizontalFlip(BYTE* Img, int W, int H) {
    for (int i = 0; i < W / 2; i++) {  // x좌표
        for (int j = 0; j < H; j++)  // y좌표
            swap(&Img[j * W + i], &Img[j * W + (W - 1 - i)]);
    }
}

// 이동
void Translation(BYTE* Image, BYTE* Output, int W, int H, int Tx, int Ty) {
    Ty *= -1; // 위아래가 바뀐 BMP을 원래대로 이동(우리가 생각하는 좌표계 기준 y축 이동)
    for (int i = 0; i < H; i++) { // y좌표
        for (int j = 0; j < W; j++) { // x좌표
            if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0)) // 원래 영상 범위 벗어났을 경우 방지
                Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j]; // 순방향
        }
    }
}

// 확대
void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y) {
    int tmpX, tmpY;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            //순방향
            /*tmpX =(int)(j * SF_X);
            tmpY = (int)(i * SF_Y);
            if (tmpY < H && tmpX < W)
               Output[tmpY * W + tmpX] = Image[i * W + j];*/

               //역방향
            tmpX = (int)(j / SF_X);
            tmpY = (int)(i / SF_Y);
            if (tmpY < H && tmpX < W)
                Output[i * W + j] = Image[tmpY * W + tmpX];
        }
    }
}

// 원점 중심 회전
void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
    int tmpX, tmpY;
    double Radian = Angle * 3.141592 / 180.0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            //순방향
            //tmpX = (int)(cos(Radian) * j - sin(Radian) * i);
            //tmpY = (int)(sin(Radian) * j + cos(Radian) * i);
            //if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
            //   Output[tmpY * W + tmpX] = Image[i * W + j];

            //역방향
            tmpX = (int)(cos(Radian) * j + sin(Radian) * i);
            tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
            if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
                Output[i * W + j] = Image[tmpY * W + tmpX];
        }
    }
}

void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
    Image[Y * W * 3 + X * 3] = B; // Blue 성분
    Image[Y * W * 3 + X * 3 + 1] = G; // Green 성분
    Image[Y * W * 3 + X * 3 + 2] = R; // Red 성분
}

void Gradation(BYTE* Image, int W) {
    double wt = 0;
    // Blue ~ Yellow
    for (int i = 0; i <= 160; i++) {
        for (int j = 0; j < W; j++) {
            wt = j / (double)(W - 1);
            Image[i * W * 3 + j * 3] = (1 - wt) * 255 + wt * 0;
            Image[i * W * 3 + j * 3 + 1] = (1 - wt) * 0 + wt * 255;
            Image[i * W * 3 + j * 3 + 2] = (1 - wt) * 0 + wt * 255;
        }
    }
    // Green ~ Magenta
    for (int i = 161; i <= 320; i++) {
        for (int j = 0; j < W; j++) {
            wt = j / (double)(W - 1);
            Image[i * W * 3 + j * 3] = (1 - wt) * 0 + wt * 255;
            Image[i * W * 3 + j * 3 + 1] = (1 - wt) * 255 + wt * 0;
            Image[i * W * 3 + j * 3 + 2] = (1 - wt) * 0 + wt * 255;
        }
    }
    // Red ~ Cyan
    for (int i = 321; i <= 480; i++) {
        for (int j = 0; j < W; j++) {
            wt = j / (double)(W - 1);
            Image[i * W * 3 + j * 3] = (1 - wt) * 0 + wt * 255;
            Image[i * W * 3 + j * 3 + 1] = (1 - wt) * 0 + wt * 255;
            Image[i * W * 3 + j * 3 + 2] = (1 - wt) * 255 + wt * 0;
        }
    }
}

void RGB2YCbCr(BYTE* Image, BYTE* Y, BYTE* Cb, BYTE* Cr, int W, int H) {
    for (int i = 0; i < H; i++) { // Y좌표
        for (int j = 0; j < W; j++) { // X좌표
            Y[i * W + j] = (BYTE)(0.299 * Image[i * W * 3 + j * 3 + 2] + 0.587 * Image[i * W * 3 + j * 3 + 1] + 0.114 * Image[i * W * 3 + j * 3]);
            Cb[i * W + j] = (BYTE)(-0.16874 * Image[i * W * 3 + j * 3 + 2] - 0.3313 * Image[i * W * 3 + j * 3 + 1] + 0.5 * Image[i * W * 3 + j * 3] + 128.0);
            Cr[i * W + j] = (BYTE)(0.5 * Image[i * W * 3 + j * 3 + 2] - 0.4187 * Image[i * W * 3 + j * 3 + 1] - 0.0813 * Image[i * W * 3 + j * 3] + 128.0);
        }
    }
}

void FindLURDColor(BYTE* Img, int W, int H, int* LRXY) {
    for (int j = 0; j < W; j++) { // X
        for (int i = 0; i < H; i++) { // Y
            if (Img[i * W * 3 + j * 3] != 0 || Img[i * W * 3 + j * 3 + 1] != 0 || Img[i * W * 3 + j * 3 + 2] != 0) {
                LRXY[0] = j;
                goto LU_Y;
            }
        }
    }

LU_Y:
    for (int i = 0; i < H; i++) { // Y
        for (int j = 0; j < W; j++) { // X
            if (Img[i * W * 3 + j * 3] != 0 || Img[i * W * 3 + j * 3 + 1] != 0 || Img[i * W * 3 + j * 3 + 2] != 0) {
                LRXY[1] = i;
                goto RD_X;
            }
        }
    }

RD_X:
    for (int j = W - 1; j >= 0; j--) { // X
        for (int i = H - 1; i >= 0; i--) { // Y
            if (Img[i * W * 3 + j * 3] != 0 || Img[i * W * 3 + j * 3 + 1] != 0 || Img[i * W * 3 + j * 3 + 2] != 0) {
                LRXY[2] = j;
                goto RD_Y;
            }
        }
    }

RD_Y:
    for (int i = H - 1; i >= 0; i--) { // Y
        for (int j = W - 1; j >= 0; j--) { // X
            if (Img[i * W * 3 + j * 3] != 0 || Img[i * W * 3 + j * 3 + 1] != 0 || Img[i * W * 3 + j * 3 + 2] != 0) {
                LRXY[3] = i;
                goto EXIT;
            }
        }
    }
EXIT:;
}

void DrawRectOutlineColor(BYTE* Img, BYTE* Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) { //외접 직사각형 그리기,좌측상단XY, 우측하단 XY
    for (int i = 0; i < H; i++) { // Y축
        for (int j = 0; j < W; j++) { // X축
            if ((i == LU_Y && j >= LU_X && j <= RD_X) || (j == LU_X && i >= LU_Y && i <= RD_Y) || (j == RD_X && i >= LU_Y && i <= RD_Y) || (i == RD_Y && j >= LU_X && j <= RD_X))
                Out[i * W * 3 + j * 3 + 2] = 255;
            else {
                Out[i * W * 3 + j * 3] = Img[i * W * 3 + j * 3];
                Out[i * W * 3 + j * 3 + 1] = Img[i * W * 3 + j * 3 + 1];
                Out[i * W * 3 + j * 3 + 2] = Img[i * W * 3 + j * 3 + 2];
            }
        }
    }
}

// 침식
void Erosion(BYTE* Image, BYTE* Output, int W, int H) {
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 255) { // 전경화소라면
                if (!(Image[(i - 1) * W + j] == 255 &&
                    Image[(i + 1) * W + j] == 255 &&
                    Image[i * W + j - 1] == 255 &&
                    Image[i * W + j + 1] == 255)) // 4주변화소가 모두 전경화소가 아니라면
                    Output[i * W + j] = 0;
                else Output[i * W + j] = 255;
            }
            else Output[i * W + j] = 0;
        }
    }
}

// 팽창
void Dilation(BYTE* Image, BYTE* Output, int W, int H) {
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 0) { // 배경화소라면
                if (!(Image[(i - 1) * W + j] == 0 &&
                    Image[(i + 1) * W + j] == 0 &&
                    Image[i * W + j - 1] == 0 &&
                    Image[i * W + j + 1] == 0)) // 4주변화소가 모두 배경화소가 아니라면
                    Output[i * W + j] = 255;
                else Output[i * W + j] = 0;
            }
            else Output[i * W + j] = 255;
        }
    }
}

void FeatureExtractThinImage(BYTE* Image, BYTE* Output, int W, int H) {
    for (int i = 1; i < H - 1; i++) { 
        for (int j = 1; j < W - 1; j++) { 
            Output[i * W + j] = Image[i * W + j];
        }
    }

    for (int i = 1; i < H - 1; i++) { 
        for (int j = 1; j < W - 1; j++) {
            int cnt = 0;
            if (Image[i * W + j] == 0 && Image[(i - 1) * W + j - 1] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[(i - 1) * W + j] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[(i - 1) * W + j + 1] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[i * W + j - 1] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[i * W + j + 1] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[(i + 1) * W + j - 1] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[(i + 1) * W + j] == 0)
                cnt++;
            if (Image[i * W + j] == 0 && Image[(i + 1) * W + j + 1] == 0)
                cnt++;

            if (cnt == 1) { // 끝점
                Output[i * W + j] = Image[i * W + j];
                Output[(i - 1) * W + j - 1] = 150;
                Output[(i - 1) * W + j] = 150;
                Output[(i - 1) * W + j + 1] = 150;
                Output[i * W + j - 1] = 150;
                Output[i * W + j + 1] = 150;
                Output[(i + 1) * W + j - 1] = 150;
                Output[(i + 1) * W + j] = 150;
                Output[(i + 1) * W + j + 1] = 150;
            }
            else if (cnt >= 3) // 분기점
                Output[i * W + j] = 200;
        }
    }
}

int main() {
    /* 영상 입력 */
    BITMAPFILEHEADER hf; // BMP 파일헤더 14Bytes
    BITMAPINFOHEADER hInfo; // BMP 영상헤더 40Bytes
    RGBQUAD hRGB[256]; // 팔레트 (256 * 4Bytes = 1024Bytes)
    FILE* fp; // 파일 포인터 : 데이터를 읽거나 내보낼 때 필요
    fp = fopen("dilation.bmp", "rb"); // 제일 첫번째 주소나 위치를 가리킴 (read binary)
    if (fp == NULL) { // 예외 검사
        printf("File not found!\n");
        return -1;
    }

    // fread함수로 정보를 읽어 메모리 변수에 담는 과정 (포인터는 함수 처리후 다음 곳을 가리킴)
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp에서 부터 파일헤더의 크기만큼(14) 읽어 hf에 저장
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp로 부터 인포헤더의 크기만큼(40) 읽어 hinfo에 저장
    // 영상의 화소 정보 읽기 (동적할당) , *malloc 함수 : 동적 할당한 메모리의 첫번째 주소 반환
    int ImgSize = hInfo.biWidth * hInfo.biHeight; // 영상의 가로 * 세로 사이즈 => 영상 전체 픽셀의 개수(영상 전체의 크기)
    int H = hInfo.biHeight, W = hInfo.biWidth;
    BYTE* Image;
    BYTE* Output;

    if (hInfo.biBitCount == 24) { // 트루 컬러
        Image = (BYTE*)malloc(ImgSize * 3); // Image 배열 : 원래 영상의 화소 정보를 담을 공간
        Output = (BYTE*)malloc(ImgSize * 3); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일)
        fread(Image, sizeof(BYTE), ImgSize * 3, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
    }
    else {
        fread(hRGB, sizeof(RGBQUAD), 256, fp); // 인덱스(그레이) fp로 부터 팔레트의 크기만큼(1024) 읽어 hRGB에 저장
        Image = (BYTE*)malloc(ImgSize); // Image 배열 : 원래 영상의 화소 정보를 담을 공간
        Output = (BYTE*)malloc(ImgSize); // Output : 영상이 처리된 결과를 담을 공간 (Image 배열과 크기 동일)
        fread(Image, sizeof(BYTE), ImgSize, fp); // 현재 fp가 가리키고 있는 곳부터 ImgSize만큼 읽어 Image에 저장(=화소 정보 읽을 첫번째 주소부터)
    }

    fclose(fp); // 파일포인터와 파일 간의 관계 끊기

    int Histo[256] = { 0, }; // 히스토그램
    int AHisto[256] = { 0, }; // 누적히스토그램

    //for (int i = 0; i < 256; i++)
    //    hRGB[i].rgbRed =
    //	hRGB[i].rgbBlue =
    //	hRGB[i].rgbGreen =
    //	hRGB[i].rgbReserved = i;

    //Erosion(Image, Output, W, H);
    //Erosion(Output, Image, W, H);
    //Erosion(Image, Output, W, H);
    //Erosion(Output, Image, W, H);
    //Erosion(Image, Output, W, H);
    //Erosion(Output, Image, W, H);
    //Erosion(Image, Output, W, H);

    Dilation(Image, Output, W, H);
    Dilation(Output, Image, W, H);
    Dilation(Image, Output, W, H);
    Erosion(Output, Image, W, H);
    Erosion(Image, Output, W, H);
    Erosion(Output, Image, W, H);
    InverseImage(Image, Image, W, H);
    zhangSuen(Image, Image, H, W);
    FeatureExtractThinImage(Image, Output, W, H);

    // (50,40)위치를 특정 색상으로
    //for (int i = 0; i < W; i++) {
    //   FillColor(Image, i, 200, W, H, 0, 255, 255);
    //}

    // (50,100) ~ (300,400) 박스 채우기
    //for (int i = 100; i <= 400; i++) {
    //   for (int j = 50; j <= 300; j++) {
    //      FillColor(Image,j, i, W, H, 255, 0, 255);
    //   }
    //}

    // 가로 띠 만들기
    // 초기화
    //for (int i = 0; i <  H; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3] = 0;
    //      Image[i * W * 3 + j * 3 + 1] = 0;
    //      Image[i * W * 3 + j * 3 + 2] = 0;
    //   }
    //}
    //// y좌표 기준 0~239 (Red)
    //for (int i = 0; i < 240; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3 + 2] = 255;
    //   }
    //}
    //// y좌표 기준 120 ~ 359 (Green)
    //for (int i = 120; i < 360; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3 + 1] = 255;
    //   }
    //}
    //// y좌표 기준 240 ~ 479 (Blue)
    //for (int i = 240; i < 480; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3] = 255;
    //   }
    //}

    // 그라데이션 만들기 (B ~ R)
    //double wt;
    //for (int a = 0; a < 120; a++) {
    //   for (int i = 0; i < W; i++) {
    //      wt = i / (double)(W - 1);
    //      Image[a * W * 3 + i * 3] = (BYTE)(255 * (1.0 - wt));  // Blue
    //      Image[a * W * 3 + i * 3 + 1] = (BYTE)(255 * (1.0 - wt)); // Green
    //      Image[a * W * 3 + i * 3 + 2] = (BYTE)(255 * wt); // Red
    //   }
    //}

    //Gradation(Image, W);

    // 빨간색 딸기영역만 masking (R, G, B 모델 기준)
    //for (int i = 0; i < H; i++) {
    //   for (int j = 0; j < W; j++) {
    //      if (Image[i * W * 3 + j * 3 + 2] > 130 &&
    //         Image[i * W * 3 + j * 3 + 1] < 50 &&
    //         Image[i * W * 3 + j * 3 + 0] < 100 ) {
    //         Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3]; // blue
    //         Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1]; // green
    //         Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2]; // red
    //      }
    //      else
    //         Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0; // black
    //   }
    //}

    //BYTE* Y = (BYTE*)malloc(ImgSize);
    //BYTE* Cb = (BYTE*)malloc(ImgSize);
    //BYTE* Cr = (BYTE*)malloc(ImgSize);

    //RGB2YCbCr(Image, Y, Cb, Cr, W, H);

    //// 빨간색 딸기영역만 masking (Y, Cb, Cr 모델 기준)
    ////for (int i = 0; i < H; i++) {
    ////    for (int j = 0; j < W; j++) {
    ////        if (Cb[i * W + j] < 140 && Cr[i * W + j] > 190) {
    ////            Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
    ////            Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
    ////            Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
    ////        }
    ////        else
    ////            Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;
    ////    }
    ////}

    //// [11주차 과제] 얼굴영역 추출 후 외접 사각형 그리기
    //for (int i = 0; i < H; i++) { //y좌표
    //    for (int j = 0; j < W; j++) { //x좌표
    //        if (Image[i * W * 3 + j * 3 + 2] > 95 && Image[i * W * 3 + j * 3 + 1] > 40 && Image[i * W * 3 + j * 3] > 20 &&
    //            Image[i * W * 3 + j * 3 + 2] > Image[i * W * 3 + j * 3 + 1] &&
    //            Image[i * W * 3 + j * 3 + 2] > Image[i * W * 3 + j * 3] &&
    //            abs(Image[i * W * 3 + j * 3 + 2] - Image[i * W * 3 + j * 3 + 1]) > 15 &&
    //            Cr[i * W + j] > 145 &&
    //            Cb[i * W + j] > 85 && Y[i * W + j] > 80 && Cr[i * W + j] <= (1.5862 * Cb[i * W + j]) + 20 &&
    //            Cr[i * W + j] >= (0.3448 * Cb[i * W + j]) + 76.2069 &&
    //            Cr[i * W + j] >= (-4.5652 * Cb[i * W + j]) + 234.5652 &&
    //            Cr[i * W + j] <= (-1.15 * Cb[i * W + j]) + 301.75 &&
    //            Cr[i * W + j] <= (-2.2857 * Cb[i * W + j]) + 432.85)
    //        {
    //            Output[i * W * 3 + j * 3] = Image[i * W * 3 + j * 3];
    //            Output[i * W * 3 + j * 3 + 1] = Image[i * W * 3 + j * 3 + 1];
    //            Output[i * W * 3 + j * 3 + 2] = Image[i * W * 3 + j * 3 + 2];
    //        }
    //        else
    //            Output[i * W * 3 + j * 3] = Output[i * W * 3 + j * 3 + 1] = Output[i * W * 3 + j * 3 + 2] = 0;

    //    }
    //}

    //int LRXY[4] = { 0, };
    //FindLURDColor(Output, W, H, LRXY);
    ////printf("%d %d %d %d", LRXY[0], LRXY[1], LRXY[2], LRXY[3]);
    //DrawRectOutlineColor(Image, Output, W, H, LRXY[0], LRXY[1], LRXY[2], LRXY[3]);

    //free(Y);
    //free(Cb);
    //free(Cr);

    /* 처리된 결과 출력 */
    SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_FeatureExtractThinImage.bmp");

    free(Image); // 동적할당 메모리 해제
    free(Output); // ..
    return 0;
}