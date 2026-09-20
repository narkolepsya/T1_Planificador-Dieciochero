#include "terminal.h"
#include <iostream>

using namespace std;

const string CIAN = "\033[36m";
const string VERDE = "\033[32m";
const string AMARILLO = "\033[33m";
const string ROJO = "\033[31m";
const string MAGENTA = "\033[35m";
const string AZUL = "\033[34m";
const string REINICIAR = "\033[0m";

void interfaz(string fileName, int K, int total)
{
    cout << endl;

    cout << CIAN;
    cout << "+--------------------------------------------------+" << endl;
    cout << "|            PLANIFICADOR DIECIOCHERO              |" << endl;
    cout << "+--------------------------------------------------+" << endl;
    cout << REINICIAR;

    cout << AMARILLO << "Archivo: "
         << REINICIAR << fileName << endl;

    cout << AMARILLO << "Procesos concurrentes: "
         << REINICIAR << K << endl;

    cout << AMARILLO << "Actividades: "
         << REINICIAR << total << endl;

    cout << CIAN;
    cout << "----------------------------------------------------" << endl;
    cout << REINICIAR << endl;
}