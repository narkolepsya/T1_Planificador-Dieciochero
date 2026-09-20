#include "scheduler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdio>

using namespace std;

const int TAM_MENSAJE = 100;

void runActivity(Actividad act)
{
    cout << "Iniciando actividad " << act.id << ": " << act.nombre
         << endl;

    // convierte milisegundos a microsegundos
    usleep(act.tiempo_ms * 1000);

    cout << "Actividad " << act.id << " terminada"
         << endl;
}

void runScheduler(map<string, Actividad> & grafo,
                  map<string, Estado> & estados,
                  int K)
{
    map<pid_t, string> actividadesActivas;
    map<pid_t, int> pipesLectura;
    map<string, string> mensajesFinalizacion;
    map<string, Actividad> :: iterator it;

    int procesosActivos = 0;
    int actividadesTerminadas = 0;
    int totalActividades = grafo.size();

    while (actividadesTerminadas < totalActividades)
    {
        bool errorCreacion = false;

        for (it = grafo.begin(); it != grafo.end(); it++)
        {
            if (procesosActivos >= K)
            {
                break;
            }

            if (ready(it -> second, estados) == true)
            {
                int canalEntrada[2];
                int canalSalida[2];

                if (pipe(canalEntrada) < 0)
                {
                    cout << "Error: no se pudo crear el pipe de entrada de la actividad "
                         << it -> first << endl;

                    errorCreacion = true;
                    break;
                }

                if (pipe(canalSalida) < 0)
                {
                    cout << "Error: no se pudo crear el pipe de salida de la actividad "
                         << it -> first << endl;

                    close(canalEntrada[0]);
                    close(canalEntrada[1]);

                    errorCreacion = true;
                    break;
                }

                pid_t pid = fork();

                if (pid < 0)
                {
                    cout << "Error: no se pudo crear el proceso de la actividad "
                         << it -> first << endl;

                    close(canalEntrada[0]);
                    close(canalEntrada[1]);
                    close(canalSalida[0]);
                    close(canalSalida[1]);

                    errorCreacion = true;
                    break;
                }

                if (pid == 0)
                {
                    // El hijo lee sus insumos y escribe su finalización
                    close(canalEntrada[1]);
                    close(canalSalida[0]);

                    int cantidadDependencias =
                        it -> second.dependencias.size();

                    for (int i = 0; i < cantidadDependencias; i++)
                    {
                        char mensajeEntrada[TAM_MENSAJE] = "";

                        ssize_t cantidadLeida =
                            read(canalEntrada[0],
                                 mensajeEntrada,
                                 TAM_MENSAJE);

                        if (cantidadLeida != TAM_MENSAJE)
                        {
                            close(canalEntrada[0]);
                            close(canalSalida[1]);
                            _exit(1);
                        }

                        cout << "Actividad " << it -> first
                             << " recibio por pipe: "
                             << mensajeEntrada << endl;
                    }

                    close(canalEntrada[0]);

                    runActivity(it -> second);

                    char mensajeSalida[TAM_MENSAJE] = "";

                    snprintf(mensajeSalida,
                             TAM_MENSAJE,
                             "Actividad %s terminada",
                             it -> second.id.c_str());

                    ssize_t cantidadEscrita =
                        write(canalSalida[1],
                              mensajeSalida,
                              TAM_MENSAJE);

                    close(canalSalida[1]);

                    if (cantidadEscrita != TAM_MENSAJE)
                    {
                        _exit(1);
                    }

                    _exit(0);
                }

                // El padre escribe los insumos y lee la finalización
                close(canalEntrada[0]);
                close(canalSalida[1]);

                int cantidadDependencias =
                    it -> second.dependencias.size();

                for (int i = 0; i < cantidadDependencias; i++)
                {
                    string idDependencia =
                        it -> second.dependencias[i];

                    char mensajeEntrada[TAM_MENSAJE] = "";

                    snprintf(mensajeEntrada,
                             TAM_MENSAJE,
                             "%s",
                             mensajesFinalizacion[idDependencia].c_str());

                    ssize_t cantidadEscrita =
                        write(canalEntrada[1],
                              mensajeEntrada,
                              TAM_MENSAJE);

                    if (cantidadEscrita != TAM_MENSAJE)
                    {
                        cout << "Error: no se pudo enviar el insumo de la actividad "
                             << idDependencia << endl;
                    }
                }

                close(canalEntrada[1]);

                estados[it -> first] = EJECUTANDO;
                actividadesActivas[pid] = it -> first;
                pipesLectura[pid] = canalSalida[0];

                procesosActivos++;
            }
        }

        if (procesosActivos > 0)
        {
            int estadoHijo;
            pid_t pidTerminado = waitpid(-1, &estadoHijo, 0);

            if (pidTerminado < 0)
            {
                cout << "Error: no se pudo esperar al proceso hijo"
                     << endl;
                return;
            }

            char mensajeSalida[TAM_MENSAJE] = "";

            ssize_t cantidadLeida =
                read(pipesLectura[pidTerminado],
                     mensajeSalida,
                     TAM_MENSAJE);

            close(pipesLectura[pidTerminado]);
            pipesLectura.erase(pidTerminado);

            string idActividad =
                actividadesActivas[pidTerminado];

            if (cantidadLeida > 0)
            {
                mensajesFinalizacion[idActividad] =
                    mensajeSalida;

                cout << "Mensaje recibido por pipe: "
                     << mensajeSalida << endl;
            }
            else
            {
                cout << "Error: no se recibió el mensaje de la actividad "
                     << idActividad << endl;
            }

            estados[idActividad] = TERMINADA;
            actividadesActivas.erase(pidTerminado);

            procesosActivos--;
            actividadesTerminadas++;
        }
        else if (errorCreacion == true)
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