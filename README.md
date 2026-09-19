<div align="center">

# Tarea 1 — Planificador Dieciochero

### Sistemas Operativos
**Universidad Diego Portales**. Escuela de Informática y Telecomunicaciones

Simulador de planificación concurrente de actividades mediante procesos, pipes y señales.

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Plataforma](https://img.shields.io/badge/Plataforma-Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Estado](https://img.shields.io/badge/Estado-En_desarrollo-D97706?style=for-the-badge)

</div>

---

## 1. Descripción del Proyecto
El siguiente proyecto denominado “Planificador Dieciochero” desarrolla un simulador orientado a organizar las actividades de celebración del señor Loyola durante toda la semana de Fiestas Patrias. Cada jornada se modela mediante un Grafo Acíclico Dirigido (DAG), en el cual las actividades pueden depender unas de otras antes de poder ejecutarse. A partir de un archivo de planificación, el programa administra la ejecución de estas actividades mediante procesos, respetando un límite máximo de concurrencia, comunicando la finalización de tareas mediante pipes y respondiendo ante fallos o ante una interrupción general.

Cada actividad posee un identificador, un nombre, una duración expresada en milisegundos y una lista de dependencias. Estas dependencias determinan qué actividades deben finalizar antes de que una nueva actividad pueda comenzar su ejecución. El programa recibe mediante línea de comandos un archivo de planificación y un valor K, correspondiente al número máximo de procesos que pueden ejecutarse de forma concurrente.

> [!IMPORTANT]
> La planificación se realiza utilizando **procesos, pipes y señales**. 

---

## 2. Estructura del proyecto

```text
T1_Planificador-Dieciochero/
├── include/
│   ├── actividad.h
│   └── parser.h
├── src/
│   ├── main.cpp
│   └── parser.cpp
└── README.md
```

- **`include/`**: contiene los archivos de cabecera (`.h`) con las declaraciones de estructuras, clases y funciones compartidas.
- **`src/`**: contiene los archivos fuente (`.cpp`) donde se implementa la lógica del programa.

---

## 3. Requisitos

Para compilar y ejecutar el programa se necesita:

- un sistema operativo compatible con procesos POSIX, como Linux;
- un compilador de C++ compatible con **C++17**;

---
