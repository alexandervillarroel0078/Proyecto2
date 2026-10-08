// AlfabetoHill.cpp
#include "AlfabetoHill.h"
#include <cctype>
#include "Tipos.h"
static const int HILL[26] = {
    5,23,2,20,10,15,8,4,18,25,0,16,13,7,3,1,19,6,12,24,21,17,14,22,11,9
};

int AlfabetoHill::letraANumero(char letra) {
    if (letra < 'A' || letra > 'Z') return -1;
    return HILL[letra - 'A'];
}

char AlfabetoHill::numeroALetra(int numero) {
    numero = ((numero % MOD) + MOD) % MOD;
    for (int i = 0; i < 26; ++i)
        if (HILL[i] == numero) return char('A' + i);
    return '?';
}

std::string AlfabetoHill::limpiar(const std::string& texto) {
    std::string out;
    for (char c : texto) {
        char u = std::toupper(static_cast<unsigned char>(c));
        if (u >= 'A' && u <= 'Z') out += u;
    }
    return out;
}

std::string AlfabetoHill::rellenar(const std::string& texto, int n, char relleno) {
    std::string out = texto;
    while (out.size() % n != 0) out += relleno;
    return out;
}