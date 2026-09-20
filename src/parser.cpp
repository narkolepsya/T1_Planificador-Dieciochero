#include "parser.h"
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <iostream>

int randomTime()
{
    return 100 + rand() % 4901;
}

string trim(string texto)
{
    if (texto.empty())
    {
        return texto;
    }

    int largo = texto.length();
    int ini = 0;
    int fin = largo - 1;

    while (ini < largo && texto[ini] == ' ')
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

Actividad parsearLinea(string linea)
{
    Actividad act;
    stringstream ss(linea);

    string id;
    string nombre;
    string tiempo;
    string dependencias;

    getline(ss, id, ':');
    getline(ss, nombre, ':');
    getline(ss, tiempo, ':');
    getline(ss, dependencias);

    act.id = trim(id);
    act.nombre = trim(nombre);
    tiempo = trim(tiempo);

    if (tiempo.empty())
    {
        act.tiempo_ms = randomTime();
    }
    else
    {
        act.tiempo_ms = stoi(tiempo);
    }
    act.dependencias = parsearDependencias(dependencias);
    return act;
}

map<string, Actividad> loadFile(string fileName)
{
    map<string, Actividad> grafo;
    ifstream archivo(fileName);
    if (!archivo.is_open())
    {
        cout << "Error al abrir el archivo " << fileName << endl;
        return grafo;
    }

    string linea;
    while (getline(archivo, linea))
    {
        Actividad act = parsearLinea(linea);
        grafo[act.id] = act;
    }
    archivo.close();
    return grafo;
}