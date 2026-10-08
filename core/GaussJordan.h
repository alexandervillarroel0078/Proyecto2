#ifndef GAUSSJORDAN_H
#define GAUSSJORDAN_H

#include "Tipos.h"

class GaussJordan {
public:
    // Invierte A mod 26. Devuelve matriz vacía si no es invertible.
    static Mat invertir(const Mat& A);
    // Recupera K = C * P^{-1} mod 26
    static Mat recuperarClave(const Mat& P, const Mat& C);

private:
    static void intercambiarFilas(Mat& M, int i, int j);
    static void multiplicarFila(Mat& M, int i, int escalar);
    static void sumarFilas(Mat& M, int destino, int origen, int escalar);
};

#endif