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
bool tieneCiclo(map<string, Actividad> & grafo);
bool revisarCiclo(string id, map<string, Actividad> & grafo,
                  map<string, bool> & visitado,
                  map<string, bool> & enCamino);
bool ready(Actividad & act, map<string, Estado> & estados);

void abortarDependientes(string idFallida, map<string, Actividad> & grafo, map<string, Estado> & estados); 

#endif