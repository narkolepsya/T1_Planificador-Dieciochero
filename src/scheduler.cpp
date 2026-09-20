#include "scheduler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void runActivity(Actividad act)
{
    cout << "Iniciando actividad " << act.id << ": " << act.nombre
    << endl;

    usleep(act.tiempo_ms * 1000); // conversion de milisegundos a microsegundos

    cout << "Actividad " << act.id << " terminada"
    << endl;
}

void runScheduler(map<string, Actividad> & grafo,
                  map<string, Estado> & estados,
                  int K)
{
    map<pid_t, string> actividadesActivas;
    map<string, Actividad> :: iterator it;

    int procesosActivos = 0;
    int actividadesTerminadas = 0;
    int totalActividades = grafo.size();

    while (actividadesTerminadas < totalActividades)
    {
        bool errorFork = false;

        for (it = grafo.begin(); it != grafo.end(); it++)
        {
            if (procesosActivos >= K)
            {
                break;
            }

            if (ready(it -> second, estados) == true)
            {
                pid_t pid = fork();

                if (pid < 0)
                {
                    cout << "Error: no se pudo crear el proceso de la actividad "
                         << it -> first << endl;

                    errorFork = true;
                    break;
                }

                if (pid == 0)
                {
                    runActivity(it -> second);
                    _exit(0);
                }

                estados[it -> first] = EJECUTANDO;
                actividadesActivas[pid] = it -> first;
                procesosActivos++;
            }
        }

        if (procesosActivos > 0)
        {
            int estadoHijo;
            pid_t pidTerminado = waitpid(-1, &estadoHijo, 0);

            if (pidTerminado < 0)
            {
                cout << "Error: no se pudo esperar al proceso hijo" << endl;
                return;
            }

            string idActividad = actividadesActivas[pidTerminado];

            estados[idActividad] = TERMINADA;
            actividadesActivas.erase(pidTerminado);

            procesosActivos--;
            actividadesTerminadas++;
        }
        else if (errorFork == true)
        {
            return;
        }
        else
        {
            cout << "Error: no hay actividades disponibles para ejecutar"
                 << endl;
            return;
        }
    }
}