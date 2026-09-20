#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>
#include "parser.h"
#include "grafo.h"
#include "scheduler.h"

using namespace std;

void iniciarEstados(map<string, Actividad>& grafo, map<string, Estado>& estados)
{
    map<string, Actividad> :: iterator it;
    for (it = grafo.begin(); it != grafo.end(); it++)
    {
        estados[it -> first] = PENDIENTE;
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
    if (K <= 0)
    {
        cout << "Error: K debe ser mayor que 0" << endl;
        return -1;
    }

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
    runScheduler(grafo, estados, K);
    return 0;
}