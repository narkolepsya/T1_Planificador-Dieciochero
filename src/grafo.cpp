#include "grafo.h"
#include <iostream>

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