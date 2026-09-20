#include "scheduler.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstdio>

using namespace std;

const int MAX_MENSAJE = 100;

volatile sig_atomic_t interrumpido = 0;

void manejarSIGINT(int)
{
    interrumpido = 1;
}

void runActivity(Actividad act)
{
    cout << "Iniciando actividad " << act.id
         << ": " << act.nombre << endl;

    // Convierte milisegundos a microsegundos
    usleep(act.tiempo_ms * 1000);

    cout << "Actividad " << act.id
         << " terminada" << endl;
}

void abortarPlan(map<pid_t, string> & actividadesActivas,
                 map<pid_t, int> & pipesLectura,
                 map<string, Estado> & estados)
{
    map<pid_t, string> :: iterator itProceso;
    map<pid_t, int> :: iterator itPipe;
    map<string, Estado> :: iterator itEstado;

    // Envía una señal de término a los hijos activos
    for (itProceso = actividadesActivas.begin();
         itProceso != actividadesActivas.end();
         itProceso++)
    {
        kill(itProceso -> first, SIGTERM);
    }

    // Espera a los hijos para no dejar procesos zombis
    for (itProceso = actividadesActivas.begin();
         itProceso != actividadesActivas.end();
         itProceso++)
    {
        waitpid(itProceso -> first, NULL, 0);
    }

    // Cierra los extremos de lectura que conserva el padre
    for (itPipe = pipesLectura.begin();
         itPipe != pipesLectura.end();
         itPipe++)
    {
        close(itPipe -> second);
    }

    // Marca como abortadas las actividades que no terminaron
    for (itEstado = estados.begin();
         itEstado != estados.end();
         itEstado++)
    {
        if (itEstado -> second == PENDIENTE ||
            itEstado -> second == EJECUTANDO)
        {
            itEstado -> second = ABORTADA;

            cout << "Actividad " << itEstado -> first
                 << " abortada" << endl;
        }
    }

    actividadesActivas.clear();
    pipesLectura.clear();
}

void runScheduler(map<string, Actividad> & grafo,
                  map<string, Estado> & estados,
                  int K)
{
    map<pid_t, string> actividadesActivas;
    map<pid_t, int> pipesLectura;
    map<string, string> mensajes;
    map<string, Actividad> :: iterator it;

    int activos = 0;
    int terminadas = 0;
    int total = grafo.size();

    interrumpido = 0;

    if (signal(SIGINT, manejarSIGINT) == SIG_ERR)
    {
        cout << "Error: no se pudo configurar SIGINT" << endl;
        return;
    }

    while (terminadas < total)
    {
        bool errorCreacion = false;

        for (it = grafo.begin(); it != grafo.end(); it++)
        {
            if (interrumpido == 1)
            {
                break;
            }

            if (activos >= K)
            {
                break;
            }

            if (ready(it -> second, estados) == true)
            {
                int canalEntrada[2];
                int canalSalida[2];

                if (pipe(canalEntrada) < 0)
                {
                    cout << "Error: no se pudo crear el pipe de entrada "
                         << "de la actividad " << it -> first << endl;

                    errorCreacion = true;
                    break;
                }

                if (pipe(canalSalida) < 0)
                {
                    cout << "Error: no se pudo crear el pipe de salida "
                         << "de la actividad " << it -> first << endl;

                    close(canalEntrada[0]);
                    close(canalEntrada[1]);

                    errorCreacion = true;
                    break;
                }

                pid_t pid = fork();

                if (pid < 0)
                {
                    cout << "Error: no se pudo crear el proceso "
                         << "de la actividad " << it -> first << endl;

                    close(canalEntrada[0]);
                    close(canalEntrada[1]);
                    close(canalSalida[0]);
                    close(canalSalida[1]);

                    errorCreacion = true;
                    break;
                }

                if (pid == 0)
                {
                    // El hijo vuelve al comportamiento normal de Ctrl+C
                    signal(SIGINT, SIG_DFL);

                    close(canalEntrada[1]);
                    close(canalSalida[0]);

                    int cantidad =
                        it -> second.dependencias.size();

                    for (int i = 0; i < cantidad; i++)
                    {
                        char mensajeEntrada[MAX_MENSAJE] = "";

                        ssize_t leidos =
                            read(canalEntrada[0],
                                 mensajeEntrada,
                                 MAX_MENSAJE);

                        if (leidos != MAX_MENSAJE)
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

                    char mensajeSalida[MAX_MENSAJE] = "";

                    snprintf(mensajeSalida,
                             MAX_MENSAJE,
                             "Actividad %s terminada",
                             it -> second.id.c_str());

                    ssize_t escritos =
                        write(canalSalida[1],
                              mensajeSalida,
                              MAX_MENSAJE);

                    close(canalSalida[1]);

                    if (escritos != MAX_MENSAJE)
                    {
                        _exit(1);
                    }

                    _exit(0);
                }

                close(canalEntrada[0]);
                close(canalSalida[1]);

                int cantidad =
                    it -> second.dependencias.size();

                for (int i = 0; i < cantidad; i++)
                {
                    string idDep =
                        it -> second.dependencias[i];

                    char mensajeEntrada[MAX_MENSAJE] = "";

                    snprintf(mensajeEntrada,
                             MAX_MENSAJE,
                             "%s",
                             mensajes[idDep].c_str());

                    ssize_t escritos =
                        write(canalEntrada[1],
                              mensajeEntrada,
                              MAX_MENSAJE);

                    if (escritos != MAX_MENSAJE)
                    {
                        cout << "Error: no se pudo enviar el insumo "
                             << "de la actividad " << idDep << endl;
                    }
                }

                close(canalEntrada[1]);

                estados[it -> first] = EJECUTANDO;
                actividadesActivas[pid] = it -> first;
                pipesLectura[pid] = canalSalida[0];

                activos++;
            }
        }

        if (interrumpido == 1)
        {
            cout << endl
                 << "Interrupcion recibida. Abortando el plan..."
                 << endl;

            abortarPlan(actividadesActivas,
                        pipesLectura,
                        estados);

            signal(SIGINT, SIG_DFL);
            return;
        }

        if (activos > 0)
        {
            int estadoHijo;
            pid_t pidFin = waitpid(-1, &estadoHijo, 0);

            if (interrumpido == 1)
            {
                cout << endl
                     << "Interrupcion recibida. Abortando el plan..."
                     << endl;

                abortarPlan(actividadesActivas,
                            pipesLectura,
                            estados);

                signal(SIGINT, SIG_DFL);
                return;
            }

            if (pidFin < 0)
            {
                cout << "Error: no se pudo esperar al proceso hijo"
                     << endl;

                signal(SIGINT, SIG_DFL);
                return;
            }

            char mensajeSalida[MAX_MENSAJE] = "";

            ssize_t leidos =
                read(pipesLectura[pidFin],
                     mensajeSalida,
                     MAX_MENSAJE);

            if (interrumpido == 1)
            {
                cout << endl
                     << "Interrupcion recibida. Abortando el plan..."
                     << endl;

                abortarPlan(actividadesActivas,
                            pipesLectura,
                            estados);

                signal(SIGINT, SIG_DFL);
                return;
            }

            close(pipesLectura[pidFin]);
            pipesLectura.erase(pidFin);

            string idAct = actividadesActivas[pidFin];

            if (leidos > 0)
            {
                mensajes[idAct] = mensajeSalida;

                cout << "Mensaje recibido por pipe: "
                     << mensajeSalida << endl;
            }
            else
            {
                cout << "Error: no se recibio el mensaje "
                     << "de la actividad " << idAct << endl;
            }

            estados[idAct] = TERMINADA;
            actividadesActivas.erase(pidFin);

            activos--;
            terminadas++;
        }
        else if (errorCreacion == true)
        {
            signal(SIGINT, SIG_DFL);
            return;
        }
        else
        {
            cout << "Error: no hay actividades disponibles para ejecutar"
                 << endl;

            signal(SIGINT, SIG_DFL);
            return;
        }
    }

    signal(SIGINT, SIG_DFL);
}