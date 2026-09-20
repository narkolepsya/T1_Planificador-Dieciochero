#include "scheduler.h"
#include "terminal.h"
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
    cout << CIAN << "[INICIO] "
         << REINICIAR << "Actividad " << act.id
         << ": " << act.nombre << endl;

    // Convierte milisegundos a microsegundos
    usleep(act.tiempo_ms * 1000);

    cout << VERDE << "[FIN] "
         << REINICIAR << "Actividad " << act.id
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

            cout << MAGENTA << "[ABORTADA] "
                 << REINICIAR << "Actividad "
                 << itEstado -> first << endl;
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
    int finalizadas = 0;
    int total = grafo.size();

    interrumpido = 0;

    if (signal(SIGINT, manejarSIGINT) == SIG_ERR)
    {
        cout << ROJO << "[ERROR] "
             << REINICIAR
             << "No se pudo configurar SIGINT"
             << endl;

        return;
    }

    while (finalizadas < total)
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
                    cout << ROJO << "[ERROR] "
                         << REINICIAR
                         << "No se pudo crear el pipe de entrada "
                         << "de la actividad " << it -> first
                         << endl;

                    errorCreacion = true;
                    break;
                }

                if (pipe(canalSalida) < 0)
                {
                    cout << ROJO << "[ERROR] "
                         << REINICIAR
                         << "No se pudo crear el pipe de salida "
                         << "de la actividad " << it -> first
                         << endl;

                    close(canalEntrada[0]);
                    close(canalEntrada[1]);

                    errorCreacion = true;
                    break;
                }

                pid_t pid = fork();

                if (pid < 0)
                {
                    cout << ROJO << "[ERROR] "
                         << REINICIAR
                         << "No se pudo crear el proceso "
                         << "de la actividad " << it -> first
                         << endl;

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

                        cout << AMARILLO << "[PIPE] "
                             << REINICIAR << "Actividad "
                             << it -> first << " recibio: "
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
                        cout << ROJO << "[ERROR] "
                             << REINICIAR
                             << "No se pudo enviar el insumo "
                             << "de la actividad " << idDep
                             << endl;
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
                 << AZUL << "[INTERRUPCION] "
                 << REINICIAR
                 << "Abortando el plan..."
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
                     << AZUL << "[INTERRUPCION] "
                     << REINICIAR
                     << "Abortando el plan..."
                     << endl;

                abortarPlan(actividadesActivas,
                            pipesLectura,
                            estados);

                signal(SIGINT, SIG_DFL);
                return;
            }

            if (pidFin < 0)
            {
                cout << ROJO << "[ERROR] "
                     << REINICIAR
                     << "No se pudo esperar al proceso hijo"
                     << endl;

                signal(SIGINT, SIG_DFL);
                return;
            }

            string idAct = actividadesActivas[pidFin];

            bool terminoBien =
                WIFEXITED(estadoHijo) &&
                WEXITSTATUS(estadoHijo) == 0;

            char mensajeSalida[MAX_MENSAJE] = "";
            ssize_t leidos = -1;

            if (terminoBien == true)
            {
                leidos = read(pipesLectura[pidFin],
                            mensajeSalida,
                            MAX_MENSAJE);
            }

            if (interrumpido == 1)
            {
                cout << endl
                     << AZUL << "[INTERRUPCION] "
                     << REINICIAR
                     << "Abortando el plan..."
                     << endl;

                abortarPlan(actividadesActivas,
                            pipesLectura,
                            estados);

                signal(SIGINT, SIG_DFL);
                return;
            }

            close(pipesLectura[pidFin]);
            pipesLectura.erase(pidFin);

            if (terminoBien == true &&
                leidos == MAX_MENSAJE)
            {
                mensajes[idAct] = mensajeSalida;

                cout << AMARILLO << "[PIPE] "
                    << REINICIAR << "Mensaje recibido: "
                    << mensajeSalida << endl;

                estados[idAct] = TERMINADA;
                finalizadas++;
            }
            else
            {
                map<string, Estado> estadosAnteriores = estados;

                estados[idAct] = FALLIDA;

                cout << ROJO << "[FALLIDA] "
                    << REINICIAR << "Actividad "
                    << idAct << endl;

                int abortadas =
                    abortarDependientes(idAct,
                                        grafo,
                                        estados);

                map<string, Estado> :: iterator itEstado;

                for (itEstado = estados.begin();
                    itEstado != estados.end();
                    itEstado++)
                {
                    if (estadosAnteriores[itEstado -> first] != ABORTADA &&
                        itEstado -> second == ABORTADA)
                    {
                        cout << MAGENTA << "[ABORTADA] "
                            << REINICIAR << "Actividad "
                            << itEstado -> first
                            << " por depender de "
                            << idAct << endl;
                    }
                }

                finalizadas += 1 + abortadas;
            }

            actividadesActivas.erase(pidFin);

            activos--;

        }
        else if (errorCreacion == true)
        {
            signal(SIGINT, SIG_DFL);
            return;
        }
        else
        {
            cout << ROJO << "[ERROR] "
                 << REINICIAR
                 << "No hay actividades disponibles para ejecutar"
                 << endl;

            signal(SIGINT, SIG_DFL);
            return;
        }
    }

    signal(SIGINT, SIG_DFL);
}