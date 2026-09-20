#include "scheduler.h"
#include <iostream>
#include <unistd.h>
using namespace std;

void runActivity(Actividad act)
{
    cout << "Iniciando actividad " << act.id << ": " << act.nombre
    << endl;

    usleep(act.tiempo_ms * 1000); // conversion de milisegundos a microsegundos

    cout << "Actividad " << act.id << " terminada"
    << endl;
}