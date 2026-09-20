#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <map>
#include <string>
#include "grafo.h"

using namespace std;

void runActivity(Actividad act);

void runScheduler(map<string, Actividad> & grafo,
                  map<string, Estado> & estados,
                  int K);

#endif