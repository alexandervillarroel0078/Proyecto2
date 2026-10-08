#include "HillCipher.h"
#include "AlfabetoHill.h"
#include "MatrizMod26.h"
#include "GaussJordan.h"

Vec HillCipher::textoAVector(const std::string& texto) {
    Vec v;
    for (size_t i = 0; i < texto.size(); ++i) {
        int n = AlfabetoHill::letraANumero(texto[i]);
        if (n >= 0) v.push_back(n);
    }
    return v;
}

std::string HillCipher::vectorATexto(const Vec& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i)
        s += AlfabetoHill::numeroALetra(v[i]);
    return s;
}

std::vector<Vec> HillCipher::dividirBloques(const Vec& v, int n) {
    std::vector<Vec> bloques;
    for (size_t i = 0; i + n <= v.size(); i += n) {
        Vec b(v.begin() + i, v.begin() + i + n);
        bloques.push_back(b);
    }
    return bloques;
}

std::string HillCipher::cifrar(const std::string& textoPlano, const Mat& K) {
    int n = (int)K.size();
    std::string limpio = AlfabetoHill::limpiar(textoPlano);
    limpio = AlfabetoHill::rellenar(limpio, n, 'X');
    Vec v = textoAVector(limpio);
    std::vector<Vec> bloques = dividirBloques(v, n);
    std::string resultado;
    for (size_t i = 0; i < bloques.size(); ++i) {
        Vec c = MatrizMod26::multiplicar(K, bloques[i]);
        resultado += vectorATexto(c);
    }
    return resultado;
}

std::string HillCipher::descifrar(const std::string& criptograma, const Mat& K) {
    int n = (int)K.size();
    Mat Kinv = GaussJordan::invertir(K);
    if (Kinv.empty()) return "";
    std::string limpio = AlfabetoHill::limpiar(criptograma);
    Vec v = textoAVector(limpio);
    std::vector<Vec> bloques = dividirBloques(v, n);
    std::string resultado;
    for (size_t i = 0; i < bloques.size(); ++i) {
        Vec p = MatrizMod26::multiplicar(Kinv, bloques[i]);
        resultado += vectorATexto(p);
    }
    return resultado;
}