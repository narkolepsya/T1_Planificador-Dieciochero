#include "parser.h"
#include <sstream>

string trim(string texto)
{
    if (texto.empty())
    {
        return texto;
    }
    int ini = 0;
    int fin = texto.length() - 1;

    while (ini < texto.length() && texto[ini] == ' ')
    {
        ini++;
    }
    while (fin >= ini && texto[fin] == ' ')
    {
        fin--;
    }
    string limpio = "";
    for (int i = ini; i <= fin; i++)
    {
        limpio += texto[i];
    }
    return limpio;
}

vector<string> parsearDependencias(string texto)
{
    vector<string> dependencias;
    texto = trim(texto);
    if (texto.empty())
    {
        return dependencias;
    }
    stringstream ss(texto);
    string dependencia;

    while (getline(ss, dependencia, ','))
    {
        dependencia = trim(dependencia);
        dependencias.push_back(dependencia);
    }
    return dependencias;
}