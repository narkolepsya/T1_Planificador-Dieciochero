#ifndef GRAFO_H
#define GRAFO_H
#include <map>
#include <string>
#include "actividad.h"
using namespace std;

enum Estado
{
    PENDIENTE,
    EJECUTANDO,
    TERMINADA,
    FALLIDA,
    ABORTADA
};

bool validate(map<string, Actividad> & grafo);

#endif