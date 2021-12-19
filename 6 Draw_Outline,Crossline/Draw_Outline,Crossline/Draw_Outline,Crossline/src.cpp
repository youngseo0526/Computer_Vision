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

// �ڽ� ��Ȱȭ
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

// ����þ� ��Ȱȭ
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
            // 0 ~ 765 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
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
            // 0 ~ 1020 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
            Out[i * W + j] = abs((long)SumProduct) / 4;
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
            // 0 ~ 1020 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
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
            // 0 ~ 765 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
            Out[i * W + j] = abs((long)SumProduct) / 3;
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
            // 0 ~ 2040 ===> 0 ~ 255�� ����ȭ (����ũ�� -1�� ���� ���� ���� 1�� ���� ���� �պ��� Ŭ ��� 255 �̻� ���� �� ����)
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
    fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp); // hf������ ������ ���Ͽ� ���
    fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp); // hinfo ..
    fwrite(hRGB, sizeof(RGBQUAD), 256, fp); // hRGB..
    fwrite(Output, sizeof(BYTE), W * H, fp); // ������ ȭ�� ���� ���(���Ӱ� ����� Output�� ���)
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
    short curColor = 0, r, c;
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
            if (curColor < 1000) BlobArea[curColor] = area;
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
        //CutImage[k] = (unsigned char)(coloring[k] * grayGap); // ���� �󺧸� ��� ����
    }

    delete[] coloring;
    delete[] stackx;
    delete[] stacky;
} // �󺧸� �� ���� ���� ������ ���ؼ��� �̾Ƴ��� �ڵ� ����

// ����ȭ �Ѵ��� ��� ����
void BinaryImageEdgeDetection(BYTE* Bin, BYTE* Out, int W, int H) {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            if (Bin[i * W + j] == 0) { // ���� ȭ�� ���
                if (!(Bin[(i - 1) * W + j] == 0 && Bin[(i + 1) * W + j] == 0 && // 4���� �ϼ� �� �ϳ��� ������ �ƴ϶�� 
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

int main()
{
    /* ���� �Է� */
    BITMAPFILEHEADER hf; // BMP ������� 14Bytes
    BITMAPINFOHEADER hInfo; // BMP ������� 40Bytes
    RGBQUAD hRGB[256]; // �ȷ�Ʈ (256 * 4Bytes = 1024Bytes)
    FILE* fp; // ���� ������ : �����͸� �аų� ������ �� �ʿ�
    fp = fopen("pupil2.bmp", "rb"); // ���� ù��° �ּҳ� ��ġ�� ����Ŵ (read binary)
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
    BYTE* Temp = (BYTE*)malloc(ImgSize); // �ӽù迭
    BYTE* Temp2 = (BYTE*)malloc(ImgSize);
    BYTE* Output = (BYTE*)malloc(ImgSize); // Output : ������ ó���� ����� ���� ���� (Image �迭�� ũ�� ����) 
    BYTE* Output2 = (BYTE*)malloc(ImgSize);
    fread(Image, sizeof(BYTE), ImgSize, fp); // ���� fp�� ����Ű�� �ִ� ������ ImgSize��ŭ �о� Image�� ����(=ȭ�� ���� ���� ù��° �ּҺ���)
    fclose(fp); // ���������Ϳ� ���� ���� ���� ����

    int H = hInfo.biHeight, W = hInfo.biWidth;
    int Histo[256] = { 0, }; // ������׷�
    int AHisto[256] = { 0, }; // ����������׷�

    Binarization(Image, Temp, W, H, 50);
    InverseImage(Temp, Temp, W, H); // ������ ��ο� �κ����� ��µǵ��� �������
    m_BlobColoring(Temp, H, W);
    for (int i = 0; i < ImgSize; i++) Output[i] = Image[i];
    //for (int i = 0; i < ImgSize; i++) Output2[i] = Image[i];
    //BinaryImageEdgeDetection(Temp, Output, W, H);

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0; // �»�, ������ xy��ǥ
    DrawRectOutline(Temp, Output, W, H, &x1, &y1, &x2, &y2); // [���� 1)]

    int X = 0, Y = 0; // �������� X, Y ��ǥ 
    DrawCrossLine(Temp, Output, W, H, &X, &Y); // [���� 2)]

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
    //int Length = 5;  // ����ũ�� �� ���� ���� (Ȧ��)
    //int Margin = Length / 2; // ������ ���͸� ó�� �� ���� ������ ���� ������
    //int WSize = Length * Length; // ����(����ũ) ������ ũ��
    //BYTE* temp = (BYTE*)malloc(sizeof(BYTE) * WSize); // BYTE������ ����ũ ũ���� �޸� �Ҵ�
    //int W = hInfo.biWidth, H = hInfo.biHeight; // ������ ����, ����
    //int i, j, m, n;  // �迭�� �ε��� ������ ���� ����
    //for (i = Margin; i < H - Margin; i++) { // ��,�� margin�� ����� ����ũ�� Y��ǥ(��)
    //   for (j = Margin; j < W - Margin; j++) { // ��,�� margin�� ����� ����ũ�� X��ǥ(��)
    //      for (m = -Margin; m <= Margin; m++) { // i�� ����Ǿ� ����ũ �߽� ���� ���ι��� �̵�(��)
    //         for (n = -Margin; n <= Margin; n++) { // j�� ����Ǿ� ����ũ �߽� ���� ���ι��� �̵�(��)
    //            temp[(m + Margin) * Length + (n + Margin)] = Image[(i + m) * W + j + n]; //
    //         }
    //      }
    //      Output[i * W + j] = Median(temp, WSize); // �߰� ���� Output�迭�� i�� j���� ����
    //   }
    //}
    //free(temp); // �����Ҵ� �޸� ����
    /* Median filtering */


    //ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);

    /* ����ó�� */
    //AverageConv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvgConv(Image, Output, hInfo.biWidth, hInfo.biHeight);

    //Prewitt_X_Conv(Image, Temp, hInfo.biWidth, hInfo.biHeight);
    //Prewitt_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //for (int i = 0; i < ImgSize; i++) { // Prewitt_X_Conv�� ������� �� �� Prewitt_Y_Conv���� ��������Ͽ� ���� ���
    //   if (Temp[i] > Output[i]) Output[i] = Temp[i];
    //}
    //Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40); // Prewitt_Conv�� ������ ��谪�� ����ȭ�Ͽ� ���

    //Sobel_X_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //Sobel_Y_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //for (int i = 0; i < ImgSize; i++) { // Sobel_X_Conv�� ������� �� �� Sobel_Y_Conv���� ��������Ͽ� ���� ���
    //   if (Temp[i] > Output[i]) Output[i] = Temp[i];
    //}
    //Binarization(Output, Output, hInfo.biWidth, hInfo.biHeight, 40); // Sobel_Conv�� ������ ��谪�� ����ȭ�Ͽ� ���

    //Laplace_Conv(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //GaussAvgConv(Image, Temp, hInfo.biWidth, hInfo.biHeight); // Laplace_Conv_DC�� ������ ������ ����� �����Ǳ� ������ ����þ� ��Ȱȭ�� ���� ��������
    //Laplace_Conv_DC(Temp, Output, hInfo.biWidth, hInfo.biHeight); // ���� ��� �����ϸ鼭 ���, ����� ������Ŵ

    //ObtainAHistogram(Histo, AHisto);
    //HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
    //HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);

    //int Thres = GozalezBinThres(Histo);

    //Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);
    //printf("Threshold by Gonzalez : %d", Thres);
    //InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
    //BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 70);
    //ContrastAdj(Image, Output , hInfo.biWidth, hInfo.biHeight, 0.5);


    /* ó���� ��� ��� */
    SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, "output_pupil2.bmp");

    free(Image); // �����Ҵ� �޸� ����
    free(Output); // ..
    free(Temp);
    return 0;
}