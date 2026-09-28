#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>

using namespace std;

extern const string CIAN;
extern const string VERDE;
extern const string AMARILLO;
extern const string ROJO;
extern const string MAGENTA;
extern const string AZUL;
extern const string REINICIAR;

enum TipoEvento
{
    EVENTO_INICIO,
    EVENTO_FIN,
    EVENTO_PIPE,
    EVENTO_ERROR,
    EVENTO_INTERRUPCION,
    EVENTO_FALLIDA,
    EVENTO_ABORTADA
};

void mostrarEvento(TipoEvento tipo, string mensaje);
void interfaz(string fileName, int K, int total);

#endif