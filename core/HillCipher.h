#ifndef HILLCIPHER_H
#define HILLCIPHER_H

#include "Tipos.h"
#include <string>

class HillCipher {
public:
    static std::string cifrar(const std::string& textoPlano, const Mat& K);
    static std::string descifrar(const std::string& criptograma, const Mat& K);
    static Vec textoAVector(const std::string& texto);
    static std::string vectorATexto(const Vec& v);
    static std::vector<Vec> dividirBloques(const Vec& v, int n);
};

#endif