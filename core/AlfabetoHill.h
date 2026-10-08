#ifndef ALFABETOHILL_H
#define ALFABETOHILL_H

#include <string>

class AlfabetoHill {
public:
    // Tabla oficial de Hill (A=5, B=23, ...)
    static int letraANumero(char letra);
    static char numeroALetra(int numero);

    // Limpieza: mayúsculas, sin espacios ni signos
    static std::string limpiar(const std::string& texto);

    // Relleno con 'X' hasta múltiplo de n
    static std::string rellenar(const std::string& texto, int n, char relleno = 'X');
};

#endif