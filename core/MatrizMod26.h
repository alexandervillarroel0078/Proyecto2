#ifndef MATRIZMOD26_H
#define MATRIZMOD26_H
#include "Tipos.h"
class MatrizMod26 {
public:
    static int mod(int x);
    static Mat identidad(int n);
    static Mat multiplicar(const Mat& A, const Mat& B);
    static Vec multiplicar(const Mat& A, const Vec& v);
    static int determinante(const Mat& A);
    static bool esInvertible(const Mat& A);
    static int inversoMod(int a, int m = MOD);
    static Mat inversaGaussJordan(const Mat& A);
    static Mat traspuesta(const Mat& A);
    static Mat adjunta(const Mat& A);
};
#endif