#include "MatrizMod26.h"
#include "GaussJordan.h"

int MatrizMod26::mod(int x) {
    int r = x % MOD;
    if (r < 0) r += MOD;
    return r;
}

Mat MatrizMod26::identidad(int n) {
    Mat I(n, Vec(n, 0));
    for (int i = 0; i < n; ++i) I[i][i] = 1;
    return I;
}

Mat MatrizMod26::multiplicar(const Mat& A, const Mat& B) {
    int n = (int)A.size();
    int p = (int)B.size();
    int m = (int)B[0].size();
    Mat C(n, Vec(m, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j) {
            long long suma = 0;
            for (int k = 0; k < p; ++k) suma += A[i][k] * B[k][j];
            C[i][j] = mod((int)(suma % MOD));
        }
    return C;
}

Vec MatrizMod26::multiplicar(const Mat& A, const Vec& v) {
    int n = (int)A.size();
    int m = (int)A[0].size();
    Vec r(n, 0);
    for (int i = 0; i < n; ++i) {
        long long suma = 0;
        for (int k = 0; k < m; ++k) suma += A[i][k] * v[k];
        r[i] = mod((int)(suma % MOD));
    }
    return r;
}

int MatrizMod26::determinante(const Mat& A) {
    int n = (int)A.size();
    if (n == 1) return mod(A[0][0]);
    if (n == 2) return mod(A[0][0]*A[1][1] - A[0][1]*A[1][0]);
    long long det = 0;
    for (int j = 0; j < n; ++j) {
        Mat sub(n - 1, Vec(n - 1, 0));
        for (int i = 1; i < n; ++i) {
            int col = 0;
            for (int k = 0; k < n; ++k) {
                if (k == j) continue;
                sub[i-1][col++] = A[i][k];
            }
        }
        int signo = (j % 2 == 0) ? 1 : -1;
        det += signo * A[0][j] * determinante(sub);
    }
    return mod((int)(det % MOD));
}

bool MatrizMod26::esInvertible(const Mat& A) {
    return inversoMod(determinante(A), MOD) != -1;
}

int MatrizMod26::inversoMod(int a, int m) {
    a = ((a % m) + m) % m;
    for (int x = 1; x < m; ++x)
        if ((a * x) % m == 1) return x;
    return -1;
}

Mat MatrizMod26::inversaGaussJordan(const Mat& A) {
    return GaussJordan::invertir(A);
}

Mat MatrizMod26::traspuesta(const Mat& A) {
    int n = (int)A.size(), m = (int)A[0].size();
    Mat T(m, Vec(n, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            T[j][i] = A[i][j];
    return T;
}

Mat MatrizMod26::adjunta(const Mat& A) {
    int n = (int)A.size();
    Mat adj(n, Vec(n, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            Mat sub(n - 1, Vec(n - 1, 0));
            int r = 0;
            for (int x = 0; x < n; ++x) {
                if (x == i) continue;
                int c = 0;
                for (int y = 0; y < n; ++y) {
                    if (y == j) continue;
                    sub[r][c++] = A[x][y];
                }
                ++r;
            }
            int signo = ((i + j) % 2 == 0) ? 1 : -1;
            adj[j][i] = mod(signo * determinante(sub));
        }
    return adj;
}