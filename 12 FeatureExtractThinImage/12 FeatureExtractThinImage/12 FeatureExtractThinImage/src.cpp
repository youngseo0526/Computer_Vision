#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>  // ����� ����ü ���� ����

// 2���� �迭 �����Ҵ� ����
unsigned char** imageMatrix;
// �������󿡼� 
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
        else Out[i] = (BYTE)(Img[i] * Val);
    }
}

// ������׷� ���ϱ�
void ObtainHistogram(BYTE* Img, int* Histo, int W, int H) {
    int ImgSize = W * H;
    for (int i = 0; i < ImgSize; i++) {
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
    for (int i = 0; i < ImgSize; i++) { // ��� ������ ȭ�Ұ��� ����ȭ������ �ٲٱ�(����)
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
int GozalezBinThres(int* Histo) {
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

// ���� ��� //
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

// �ڽ� ��Ȱȭ(���)
void AverageConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.11111, 0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111,
                      0.11111 ,0.11111 ,0.11111 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

// ����þ� ��Ȱȭ(�ڽ� ��Ȱȭ���� ������ �� ����)
void GaussAvgConv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { 0.0625, 0.125 ,0.0625,
                      0.125 ,0.25 ,0.125,
                      0.0625 ,0.125 ,0.0625 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            Out[i * W + j] = (BYTE)SumProduct;
            SumProduct = 0.0;
        }
    }
}

// Prewitt - X ����ũ
void Prewitt_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, 0.0 ,1.0,
                      -1.0 ,0.0 ,1.0,
                      -1.0 ,0.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            // 0 ~ 765 ==> 0 ~ 255�� ����ȭ
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

// Prewitt - Y ����ũ
void Prewitt_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      0.0 ,0.0 ,0.0,
                      1.0 ,1.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            // 0 ~ 765 ==> 0 ~ 255�� ����ȭ
            Out[i * W + j] = abs((long)SumProduct) / 3;
            SumProduct = 0.0;
        }
    }
}

// Sobel - X ����ũ
void Sobel_X_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, 0.0 ,1.0,
                      -2.0 ,0.0 ,2.0,
                      -1.0 ,0.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            // 0 ~ 1024 ===> 0 ~ 255�� ����ȭ
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

// Sobel - Y ����ũ
void Sobel_Y_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -2.0 ,-1.0,
                      0.0 ,0.0 ,0.0,
                      1.0 ,2.0 ,1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            // 0 ~ 1024 ===> 0 ~ 255�� ����ȭ
            Out[i * W + j] = abs((long)SumProduct) / 4;
            SumProduct = 0.0;
        }
    }
}

// Laplacian ����ũ
void Laplace_Conv(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      -1.0, 8.0 ,-1.0,
                      -1.0 ,-1.0 ,-1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
                }
            }
            // 0 ~ 2048 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
            Out[i * W + j] = abs((long)SumProduct) / 8;
            SumProduct = 0.0;
        }
    }
}

// ���� ��⸦ ������ Laplacian ����ũ
void Laplace_Conv_DC(BYTE* Img, BYTE* Out, int W, int H) {
    double Kernel[3][3] = { -1.0, -1.0 ,-1.0,
                      -1.0, 9.0 ,-1.0, // ���� ��� ���� ���� Ŀ�� ���� 1�� ���� 
                      -1.0 ,-1.0 ,-1.0 };
    double SumProduct = 0.0;
    for (int i = 1; i < H - 1; i++) { // Kernel Cetner�� Y��ǥ(��) (������ ���� ���� ���� 1���� ����, -1���� ����)
        for (int j = 1; j < W - 1; j++) { // Kernel Center�� X��ǥ(��)
            for (int m = -1; m <= 1; m++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                for (int n = -1; n <= 1; n++) { // Kernel �߽� ���� ���ι��� �̵�(��)
                   /* i*W + j : 2�����迭���� 1���������� �ε��� �ϴ� �� */
                    SumProduct += Img[(i + m) * W + (j + n)] * Kernel[m + 1][n + 1]; // �ε����� ������ �� ���� ������ +1����
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

// ó���� ��� ���
void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo,
    RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName) {
    FILE* fp = fopen(FileName, "wb"); // ����ִ� ���� ù��° �ּҳ� ��ġ ����Ŵ (write binary)
    if (hInfo.biBitCount == 24) {
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
        fwrite(Output, sizeof(BYTE), W * H * 3, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
    }
    else if (hInfo.biBitCount == 8) {
        fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
        fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
        fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
        fwrite(Output, sizeof(BYTE), W * H, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
    }
    fclose(fp);
}

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

// �ִ밪 ���͸�(����)
BYTE MaxPooling(BYTE* arr, int size) {
    // �������� ����
    const int S = size; // ���ͷ� ���
    for (int i = 0; i < size - 1; i++) { // pivot index
        for (int j = i + 1; j < size; j++) { // �񱳴�� index
            if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
        }
    }
    return arr[S - 1];
}

// �ּҰ� ���͸�(����)
BYTE MinPooling(BYTE* arr, int size) {
    // �������� ����
    const int S = size; // ���ͷ� ���
    for (int i = 0; i < size - 1; i++) { // pivot index
        for (int j = i + 1; j < size; j++) { // �񱳴�� index
            if (arr[i] > arr[j]) swap(&arr[i], &arr[j]);
        }
    }
    return arr[0];
}

// �󺧸� �Լ����� ���̴� �Լ�
int push(short* stackx, short* stacky, int arr_size, short vx, short vy, int* top) {
    if (*top >= arr_size) return(-1);
    (*top)++;
    stackx[*top] = vx;
    stacky[*top] = vy;
    return(1);
}

// �󺧸� �Լ����� ���̴� �Լ�
int pop(short* stackx, short* stacky, short* vx, short* vy, int* top) {
    if (*top == 0) return(-1);
    *vx = stackx[*top];
    *vy = stacky[*top];
    (*top)--;
    return(1);
}

// GlassFire �˰����� �̿��� �󺧸� �Լ�
void m_BlobColoring(BYTE* CutImage, int height, int width) {
    int i, j, m, n, top, area, Out_Area, index, BlobArea[1000];
    long k;
    short curColor = 0, r, c; // curColor: �� component ����
    //   BYTE** CutImage2;
    Out_Area = 1;


    // �������� ����� �޸� �Ҵ�
    short* stackx = new short[height * width];
    short* stacky = new short[height * width];
    short* coloring = new short[height * width];

    int arr_size = height * width;

    // �󺧸��� �ȼ��� �����ϱ� ���� �޸� �Ҵ�
    for (k = 0; k < height * width; k++) coloring[k] = 0;  // �޸� �ʱ�ȭ
    for (i = 0; i < height; i++) {
        index = i * width;
        for (j = 0; j < width; j++) {
            // �̹� �湮�� ���̰ų� �ȼ����� 255�� �ƴ϶�� ó�� ����
            if (coloring[index + j] != 0 || CutImage[index + j] != 255) continue;
            r = i; c = j; top = 0; area = 1;
            curColor++;

            while (1) {
            GRASSFIRE:
                for (m = r - 1; m <= r + 1; m++) {
                    index = m * width;
                    for (n = c - 1; n <= c + 1; n++) {
                        //���� �ȼ��� �����踦 ����� ó�� ����
                        if (m < 0 || m >= height || n < 0 || n >= width) continue;

                        if ((int)CutImage[index + n] == 255 && coloring[index + n] == 0) {
                            coloring[index + n] = curColor; // ���� �󺧷� ��ũ
                            if (push(stackx, stacky, arr_size, (short)m, (short)n, &top) == -1) continue;
                            r = m; c = n; area++;
                            goto GRASSFIRE;
                        }
                    }
                }
                if (pop(stackx, stacky, &r, &c, &top) == -1) break;
            }
            if (curColor < 1000) BlobArea[curColor] = area; // labeling�� area �迭
        }
    }

    float grayGap = 255.0f / (float)curColor;

    // ���� ������ ���� ������ ã�Ƴ��� ���� 
    for (i = 1; i <= curColor; i++) {
        if (BlobArea[i] >= BlobArea[Out_Area]) Out_Area = i;
    }
    // CutImage �迭 Ŭ����~
    for (k = 0; k < width * height; k++) CutImage[k] = 255;

    // coloring�� ����� �󺧸� ����� (Out_Area�� �����) ������ ���� ū �͸� CutImage�� ����
    for (k = 0; k < width * height; k++) {
        if (coloring[k] == Out_Area) CutImage[k] = 0;  // ���� ū �͸� ���� (size filtering)
        //if (BlobArea[coloring[k]] > 500) CutImage[k] = 0;  // Ư�� ���� �̻�Ǵ� ������ ���
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap); // ���� �󺧸� ��� ���� (255�� �󺧸��� ��������(25��)�� ���� ����)
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
}
// �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����

// ����ȭ �Ѵ��� ��� ����
void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Bin[i * W + j] == 0) { // ���� ȭ�� ���
                if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && // ���� ȭ�� �� �ֺ� 4���� ȭ�Ұ� �ϳ��� ������ �ƴϸ� ���
                    Bin[i * W + (j - 1)] == 0 && Bin[i * W + (j + 1)] == 0))
                    Out[i * W + j] = 255;
            }
        }
    }
}

// [���� 1)] ���� ������ �����ϴ� ���簢���� �׸���
void DrawRectOutline(BYTE* Img, BYTE* Out, int W, int H, int* LU_X, int* LU_Y, int* RD_X, int* RD_Y) {
    // 1) ������ܺ��� �Ϲ��� ���ο켱����  Ž���ϴٰ�, 0��ȭ�Ҹ� ó�� ������ ���� X��ǥ�� ���� : x1
    for (int j = W - 1; j >= 0; j--) {
        for (int i = H - 1; i >= 0; i--) {
            if (Img[i * W + j] == 0)
                *LU_X = j;
        }
    }
    // 2) �����ϴܺ��� �¹��� ���ο켱���� Ž���ϴٰ�, 0��ȭ�Ҹ� ó�� ������ ���� Y��ǥ�� ���� : y1
    for (int i = H - 1; i >= 0; i--) {
        for (int j = W - 1; j >= 0; j--) {
            if (Img[i * W + j] == 0)
                *LU_Y = i;
        }
    }
    // 3) ������ܺ��� �Ϲ��� ���ο켱����  Ž���ϴٰ�, 0��ȭ�Ҹ� ó�� ������ ���� X��ǥ�� ���� : x2
    for (int j = 0; j < W; j++) {
        for (int i = 0; i < H; i++) {
            if (Img[i * W + j] == 0)
                *RD_X = j;
        }
    }
    // 4) �����ϴܺ��� ����� ���ο켱����  Ž���ϴٰ�, 0��ȭ�Ҹ� ó�� ������ ���� X��ǥ�� ���� : y2
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Img[i * W + j] == 0)
                *RD_Y = i;
        }
    }
    //printf("%d  %d  %d  %d", *LU_X, *LU_Y, *RD_X, *RD_Y);

    // ��ǥ ����Ͽ� ���簢�� �׸���
    for (int i = 0; i < H; i++) { //y��
        for (int j = 0; j < W; j++) { //x��
            if ((j == *LU_X && i > *LU_Y && i < *RD_Y) || (i == *LU_Y && j > *LU_X && j < *RD_X) || (j == *RD_X && i > *LU_Y && i < *RD_Y) || (i == *RD_Y && j > *LU_X && j < *RD_X))
                Out[i * W + j] = 255;
        }
    }
}

// [���� 2)] ���� ������ �����߽��� ����ϴ� ����/������ �׸���
void DrawCrossLine(BYTE* Img, BYTE* Out, int W, int H, int* Cx, int* Cy) {
    // ���� ������ �����߽� ã��
    int count = 0;
    for (int i = 0; i < H; i++) {  // Y��ǥ
        for (int j = 0; j < W; j++) { // X��ǥ
            if (Img[i * W + j] == 0) {
                *Cx += j;
                *Cy += i;
                count++;
            }
        }
    }
    *Cx /= count;
    *Cy /= count;

    // �����߽� ���� ����/������ �׸���
    for (int i = 0; i < H; i++) { // Y��
        for (int j = 0; j < W; j++) { // X��
            if (j == *Cx || i == *Cy)
                Out[i * W + j] = 255;
        }
    }
}

// ��,�Ʒ� ��ȯ
void VerticalFilp(BYTE* Img, int W, int H) {
    for (int i = 0; i < H / 2; i++) {  // y��ǥ
        for (int j = 0; j < W; j++)  // x��ǥ
            swap(&Img[i * W + j], &Img[(H - 1 - i) * W + j]);
    }
}

// ��, �� ��ȯ
void HorizontalFlip(BYTE* Img, int W, int H) {
    for (int i = 0; i < W / 2; i++) {  // x��ǥ
        for (int j = 0; j < H; j++)  // y��ǥ
            swap(&Img[j * W + i], &Img[j * W + (W - 1 - i)]);
    }
}

// �̵�
void Translation(BYTE* Image, BYTE* Output, int W, int H, int Tx, int Ty) {
    Ty *= -1; // ���Ʒ��� �ٲ� BMP�� ������� �̵�(�츮�� �����ϴ� ��ǥ�� ���� y�� �̵�)
    for (int i = 0; i < H; i++) { // y��ǥ
        for (int j = 0; j < W; j++) { // x��ǥ
            if ((i + Ty < H && i + Ty >= 0) && (j + Tx < W && j + Tx >= 0)) // ���� ���� ���� ����� ��� ����
                Output[(i + Ty) * W + (j + Tx)] = Image[i * W + j]; // ������
        }
    }
}

// Ȯ��
void Scaling(BYTE* Image, BYTE* Output, int W, int H, double SF_X, double SF_Y) {
    int tmpX, tmpY;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            //������
            /*tmpX =(int)(j * SF_X);
            tmpY = (int)(i * SF_Y);
            if (tmpY < H && tmpX < W)
               Output[tmpY * W + tmpX] = Image[i * W + j];*/

               //������
            tmpX = (int)(j / SF_X);
            tmpY = (int)(i / SF_Y);
            if (tmpY < H && tmpX < W)
                Output[i * W + j] = Image[tmpY * W + tmpX];
        }
    }
}

// ���� �߽� ȸ��
void Rotation(BYTE* Image, BYTE* Output, int W, int H, int Angle) {
    int tmpX, tmpY;
    double Radian = Angle * 3.141592 / 180.0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            //������
            //tmpX = (int)(cos(Radian) * j - sin(Radian) * i);
            //tmpY = (int)(sin(Radian) * j + cos(Radian) * i);
            //if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
            //   Output[tmpY * W + tmpX] = Image[i * W + j];

            //������
            tmpX = (int)(cos(Radian) * j + sin(Radian) * i);
            tmpY = (int)(-sin(Radian) * j + cos(Radian) * i);
            if ((tmpY < H && tmpY >= 0) && (tmpX < W && tmpX >= 0))
                Output[i * W + j] = Image[tmpY * W + tmpX];
        }
    }
}

void FillColor(BYTE* Image, int X, int Y, int W, int H, BYTE R, BYTE G, BYTE B) {
    Image[Y * W * 3 + X * 3] = B; // Blue ����
    Image[Y * W * 3 + X * 3 + 1] = G; // Green ����
    Image[Y * W * 3 + X * 3 + 2] = R; // Red ����
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
    for (int i = 0; i < H; i++) { // Y��ǥ
        for (int j = 0; j < W; j++) { // X��ǥ
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

void DrawRectOutlineColor(BYTE* Img, BYTE* Out, int W, int H, int LU_X, int LU_Y, int RD_X, int RD_Y) { //���� ���簢�� �׸���,�������XY, �����ϴ� XY
    for (int i = 0; i < H; i++) { // Y��
        for (int j = 0; j < W; j++) { // X��
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

// ħ��
void Erosion(BYTE* Image, BYTE* Output, int W, int H) {
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 255) { // ����ȭ�Ҷ��
                if (!(Image[(i - 1) * W + j] == 255 &&
                    Image[(i + 1) * W + j] == 255 &&
                    Image[i * W + j - 1] == 255 &&
                    Image[i * W + j + 1] == 255)) // 4�ֺ�ȭ�Ұ� ��� ����ȭ�Ұ� �ƴ϶��
                    Output[i * W + j] = 0;
                else Output[i * W + j] = 255;
            }
            else Output[i * W + j] = 0;
        }
    }
}

// ��â
void Dilation(BYTE* Image, BYTE* Output, int W, int H) {
    for (int i = 1; i < H - 1; i++) {
        for (int j = 1; j < W - 1; j++) {
            if (Image[i * W + j] == 0) { // ���ȭ�Ҷ��
                if (!(Image[(i - 1) * W + j] == 0 &&
                    Image[(i + 1) * W + j] == 0 &&
                    Image[i * W + j - 1] == 0 &&
                    Image[i * W + j + 1] == 0)) // 4�ֺ�ȭ�Ұ� ��� ���ȭ�Ұ� �ƴ϶��
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

            if (cnt == 1) { // ����
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
            else if (cnt >= 3) // �б���
                Output[i * W + j] = 200;
        }
    }
}

int main() {
    /* ���� �Է� */
    BITMAPFILEHEADER hf; // BMP ������� 14Bytes
    BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
    RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes = 1024Bytes)
    FILE* fp; // ���� ������ : �����͸� �аų� ������ �� �ʿ�
    fp = fopen("dilation.bmp", "rb"); // ���� ù��° �ּҳ� ��ġ�� ����Ŵ (read binary)
    if (fp == NULL) { // ���� �˻�
        printf("File not found!\n");
        return -1;
    }

    // fread�Լ��� ������ �о� �޸� ������ ��� ���� (�����ʹ� �Լ� ó���� ���� ���� ����Ŵ)
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp); // fp���� ���� ��������� ũ�⸸ŭ(14) �о� hf�� ����
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp); // fp�� ���� ��������� ũ�⸸ŭ(40) �о� hinfo�� ����
    // ������ ȭ�� ���� �б� (�����Ҵ�) , *malloc �Լ� : ���� �Ҵ��� �޸��� ù��° �ּ� ��ȯ
    int ImgSize = hInfo.biWidth * hInfo.biHeight; // ������ ���� * ���� ������ => ���� ��ü �ȼ��� ����(���� ��ü�� ũ��)
    int H = hInfo.biHeight, W = hInfo.biWidth;
    BYTE* Image;
    BYTE* Output;

    if (hInfo.biBitCount == 24) { // Ʈ�� �÷�
        Image = (BYTE*)malloc(ImgSize * 3); // Image �迭 : ���� ������ ȭ�� ������ ���� ����
        Output = (BYTE*)malloc(ImgSize * 3); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����)
        fread(Image, sizeof(BYTE), ImgSize * 3, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
    }
    else {
        fread(hRGB, sizeof(RGBQUAD), 256, fp); // �ε���(�׷���) fp�� ���� �ȷ�Ʈ�� ũ�⸸ŭ(1024) �о� hRGB�� ����
        Image = (BYTE*)malloc(ImgSize); // Image �迭 : ���� ������ ȭ�� ������ ���� ����
        Output = (BYTE*)malloc(ImgSize); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����)
        fread(Image, sizeof(BYTE), ImgSize, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
    }

    fclose(fp); // ���������Ϳ� ���� ���� ���� ����

    int Histo[256] = { 0, }; // ������׷�
    int AHisto[256] = { 0, }; // ����������׷�

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

    // (50,40)��ġ�� Ư�� ��������
    //for (int i = 0; i < W; i++) {
    //   FillColor(Image, i, 200, W, H, 0, 255, 255);
    //}

    // (50,100) ~ (300,400) �ڽ� ä���
    //for (int i = 100; i <= 400; i++) {
    //   for (int j = 50; j <= 300; j++) {
    //      FillColor(Image,j, i, W, H, 255, 0, 255);
    //   }
    //}

    // ���� �� �����
    // �ʱ�ȭ
    //for (int i = 0; i <  H; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3] = 0;
    //      Image[i * W * 3 + j * 3 + 1] = 0;
    //      Image[i * W * 3 + j * 3 + 2] = 0;
    //   }
    //}
    //// y��ǥ ���� 0~239 (Red)
    //for (int i = 0; i < 240; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3 + 2] = 255;
    //   }
    //}
    //// y��ǥ ���� 120 ~ 359 (Green)
    //for (int i = 120; i < 360; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3 + 1] = 255;
    //   }
    //}
    //// y��ǥ ���� 240 ~ 479 (Blue)
    //for (int i = 240; i < 480; i++) {
    //   for (int j = 0; j < W; j++) {
    //      Image[i * W * 3 + j * 3] = 255;
    //   }
    //}

    // �׶��̼� ����� (B ~ R)
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

    // ������ ���⿵���� masking (R, G, B �� ����)
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

    //// ������ ���⿵���� masking (Y, Cb, Cr �� ����)
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

    //// [11���� ����] �󱼿��� ���� �� ���� �簢�� �׸���
    //for (int i = 0; i < H; i++) { //y��ǥ
    //    for (int j = 0; j < W; j++) { //x��ǥ
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

    /* ó���� ��� ��� */
    SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_FeatureExtractThinImage.bmp");

    free(Image); // �����Ҵ� �޸� ����
    free(Output); // ..
    return 0;
}