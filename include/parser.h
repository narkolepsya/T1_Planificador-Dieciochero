#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <map>
#include "actividad.h"
using namespace std;

string trim(string texto);
vector<string> parsearDependencias(string texto);
Actividad parsearLinea(string linea);
map<string, Actividad> loadFile(string fileName);

#endif