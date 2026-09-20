#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include "parser.h"
#include "grafo.h"
using namespace std;

void iniciarEstados(map<string, Actividad>& grafo, map<string, Estado>& estados)
{
    map<string, Actividad> :: iterator it;
    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        estados[it -> first] = PENDIENTE;
    }
}

void mostrarReady(map<string, Actividad>& grafo, map<string, Estado>& estados)
{
    map<string, Actividad> :: iterator it;
    cout << "Actividades listas al inicio:" << endl;
    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        if (ready(it -> second, estados) == true)
        {
            cout << it -> first << endl;
        }
    }
}

void mostrarGrafo(map<string, Actividad>& grafo)
{
    map<string, Actividad> :: iterator it;
    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        cout << "ID: " << it -> second.id << endl;
        cout << "Nombre: " << it -> second.nombre << endl;
        cout << "Tiempo: " << it -> second.tiempo_ms << endl;
        cout << "Dependencias: ";

        int cantidadDependencias = it -> second.dependencias.size();

        for (int i = 0; i < cantidadDependencias; i++)
        {
            cout << it -> second.dependencias[i] << " ";
        }

        cout << endl << endl;
    }
}

int main(int argc, char* argv[])
{
    string fileName;
    int K;
    map<string, Actividad> grafo;
    map<string, Estado> estados;

    if (argc != 3)
    {
        cout << "Uso: ./planificador <archivo.txt> <K>" << endl;
        return -1;
    }

    srand(time(NULL));
    fileName = argv[1];
    K = stoi(argv[2]);
    grafo = loadFile(fileName);

    if (validate(grafo) == false)
    {
        return -1;
    }

    if (tieneCiclo(grafo) == true)
    {
        cout << "Error: ciclo circular de dependencias" << endl;
        return -1;
    }

    iniciarEstados(grafo, estados);
    mostrarReady(grafo, estados);
    cout << "K: " << K << endl;
    mostrarGrafo(grafo);
    return 0;
}