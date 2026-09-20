#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include "parser.h"
using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Uso: ./planificador <archivo.txt> <K>" << endl;
        return -1;
    }

    srand(time(NULL));
    string fileName = argv[1];
    int K = stoi(argv[2]);
    map<string, Actividad> grafo = loadFile(fileName);
    cout << "K: " << K << endl;
    map<string, Actividad> :: iterator it;

    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        cout << "ID: " << it -> second.id << endl;
        cout << "Nombre: " << it -> second.nombre << endl;
        cout << "Tiempo: " << it -> second.tiempo_ms << endl;
        cout << "Dependencias: ";
        for (int i = 0; i < it -> second.dependencias.size(); i++)
        {
            cout << it -> second.dependencias[i] << " ";
        }
        cout << endl << endl;
    }
    return 0;
}