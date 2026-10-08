#include "GaussJordan.h"
#include "MatrizMod26.h"
#include <algorithm>

void GaussJordan::intercambiarFilas(Mat& M, int i, int j) {
    std::swap(M[i], M[j]);
}

void GaussJordan::multiplicarFila(Mat& M, int i, int escalar) {
    for (size_t k = 0; k < M[i].size(); ++k)
        M[i][k] = MatrizMod26::mod(M[i][k] * escalar);
}

void GaussJordan::sumarFilas(Mat& M, int destino, int origen, int escalar) {
    for (size_t k = 0; k < M[destino].size(); ++k)
        M[destino][k] = MatrizMod26::mod(M[destino][k] + escalar * M[origen][k]);
}

Mat GaussJordan::invertir(const Mat& A) {
    int n = (int)A.size();
    if (n == 0 || (int)A[0].size() != n) return Mat();

    // Construir [A | I]
    Mat M(n, Vec(2 * n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            M[i][j] = MatrizMod26::mod(A[i][j]);
        M[i][n + i] = 1;
    }

    // Gauss-Jordan
    for (int col = 0; col < n; ++col) {
        // Buscar pivote coprimo con 26
        int piv = -1;
        for (int f = col; f < n; ++f) {
            if (MatrizMod26::inversoMod(M[f][col], MOD) != -1) {
                piv = f; break;
            }
        }
        if (piv == -1) return Mat();

        if (piv != col) intercambiarFilas(M, col, piv);

        // Normalizar pivote a 1
        int inv = MatrizMod26::inversoMod(M[col][col], MOD);
        multiplicarFila(M, col, inv);

        // Anular otras filas
        for (int f = 0; f < n; ++f) {
            if (f == col) continue;
            int factor = M[f][col];
            if (factor != 0) sumarFilas(M, f, col, -factor);
        }
    }

    // Extraer mitad derecha
    Mat Inv(n, Vec(n, 0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            Inv[i][j] = M[i][n + j];
    return Inv;
}

Mat GaussJordan::recuperarClave(const Mat& P, const Mat& C) {
    Mat Pinv = invertir(P);
    if (Pinv.empty()) return Mat();
    return MatrizMod26::multiplicar(C, Pinv);
}