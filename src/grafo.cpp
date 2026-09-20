#include "grafo.h"
#include <iostream>
#include <map>

bool validate(map<string, Actividad> & grafo)
{
    map<string, Actividad> :: iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        int cantidad = it -> second.dependencias.size();

        for (int i = 0; i < cantidad; i++)
        {
            string dependencia = it -> second.dependencias[i];

            if (grafo.count(dependencia) == 0)
            {
                cout << "Error: la actividad " << it -> first
                     << " depende de una actividad inexistente: "
                     << dependencia << endl;

                return false;
            }
        }
    }

    return true;
}

bool revisarCiclo(string id, map<string, Actividad> & grafo,
                  map<string, bool> & visitado,
                  map<string, bool> & enCamino)
{
    if (enCamino[id] == true)
    {
        return true;
    }
    if (visitado[id] == true)
    {
        return false;
    }
    visitado[id] = true;
    enCamino[id] = true;

    int cantidad = grafo[id].dependencias.size();
    for (int i = 0; i < cantidad; i++)
    {
        string dependencia = grafo[id].dependencias[i];

        if (revisarCiclo(dependencia, grafo, visitado, enCamino) == true)
        {
            return true;
        }
    }
    enCamino[id] = false;
    return false;
}

bool tieneCiclo(map<string, Actividad> & grafo)
{
    map<string, bool> visitado;
    map<string, bool> enCamino;
    map<string, Actividad> :: iterator it;
    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        if (visitado[it -> first] == false)
        {
            if (revisarCiclo(it -> first, grafo, visitado, enCamino) == true)
            {
                return true;
            }
        }
    }
    return false;
}

// devuelve true si la actividad está lista para ejecutarse, false en caso contrario
bool ready(Actividad& act, map<string, Estado>& estados)
{
    if (estados[act.id] != PENDIENTE)
    {
        return false;
    }

    int cantidad = act.dependencias.size();

    for (int i = 0; i < cantidad; i++)
    {
        string dependencia = act.dependencias[i];

        if (estados[dependencia] != TERMINADA)
        {
            return false;
        }
    }

    return true;
}

int abortarDependientes(string idFallida,
                        map<string, Actividad> & grafo,
                        map<string, Estado> & estados)
{
    map<string, Actividad> :: iterator it;
    int abortadas = 0;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        string idActual = it -> first;

        if (estados[idActual] != PENDIENTE)
        {
            continue;
        }

        int cantidad = it -> second.dependencias.size();
        bool esHija = false;

        for (int i = 0; i < cantidad; i++)
        {
            if (it -> second.dependencias[i] == idFallida)
            {
                esHija = true;
                break;
            }
        }

        if (esHija == true)
        {
            estados[idActual] = ABORTADA;
            abortadas++;

            abortadas += abortarDependientes(idActual,
                                              grafo,
                                              estados);
        }
    }

    return abortadas;
}