<div align="center">

# Tarea 1 — Planificador Dieciochero

Simulador de planificación concurrente de actividades mediante procesos, pipes y señales.

---

**Universidad Diego Portales** · Escuela de Informática y Telecomunicaciones

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![GNU Make](https://img.shields.io/badge/GNU_Make-A42E2B?style=for-the-badge&logo=gnu&logoColor=white)
![Plataforma](https://img.shields.io/badge/Plataforma-Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Estado](https://img.shields.io/badge/Estado-Finalizado-2E8B57?style=for-the-badge)

</div>

---

## 1. Descripción del proyecto

El siguiente proyecto denominado **“Planificador Dieciochero”** desarrolla un simulador orientado a organizar las actividades de celebración del señor Loyola durante toda la semana de Fiestas Patrias. Cada jornada se modela mediante un Grafo Acíclico Dirigido (DAG), en el cual las actividades pueden depender unas de otras antes de poder ejecutarse. A partir de un archivo de planificación, el programa administra la ejecución de estas actividades mediante procesos, respetando un límite máximo de concurrencia, comunicando la finalización de tareas mediante pipes y respondiendo ante fallos o una interrupción general.

Cada actividad posee un identificador, un nombre, una duración expresada en milisegundos y una lista de dependencias. Estas dependencias determinan qué actividades deben finalizar antes de que una nueva actividad pueda comenzar su ejecución. El programa recibe mediante línea de comandos un archivo de planificación y un valor `K`, correspondiente al número máximo de procesos que pueden ejecutarse de forma concurrente.

> [!IMPORTANT]
> La planificación se realiza utilizando **procesos, pipes y señales**, sin utilizar threads ni mecanismos de sincronización asociados a hilos.

---

## 2. Estructura del proyecto

```text
T1_Planificador-Dieciochero/
├── include/
│   ├── actividad.h
│   ├── grafo.h
│   ├── parser.h
│   ├── scheduler.h
│   └── terminal.h
├── src/
│   ├── grafo.cpp
│   ├── main.cpp
│   ├── parser.cpp
│   ├── scheduler.cpp
│   └── terminal.cpp
├── .gitignore
├── Makefile
└── README.md
```

- **`include/`**: contiene los archivos de cabecera (`.h`) con las declaraciones de estructuras y funciones compartidas.
- **`src/`**: contiene los archivos fuente (`.cpp`) donde se implementa la lógica del programa.
- **`Makefile`**: centraliza las reglas y opciones necesarias para compilar el proyecto.
- **`.gitignore`**: excluye el ejecutable y los archivos objeto generados durante la compilación.

La solución se divide en módulos para mantener separadas sus responsabilidades:

| Módulo | Responsabilidad |
|:---|:---|
| `actividad` | Define la información de cada actividad. |
| `parser` | Lee el archivo de entrada y procesa sus campos. |
| `grafo` | Valida dependencias, detecta ciclos, determina qué actividades están listas y aborta las ramas dependientes de una actividad fallida. |
| `scheduler` | Administra procesos, pipes, concurrencia, estados y señales. |
| `terminal` | Presenta el encabezado y los mensajes visuales de la ejecución. |
| `main` | Valida los argumentos y coordina la ejecución general. |

---

## 3. Requisitos

Para compilar y ejecutar el programa se necesita:

- un sistema operativo compatible con procesos POSIX, como Linux;
- un compilador de C++ compatible con **C++17**;
- **GNU Make**.

---

## 4. Compilación

La compilación debe realizarse desde la carpeta raíz del proyecto:

```bash
make
```

Este comando compila todos los archivos fuente y genera el ejecutable `planificador`. Durante la compilación también se generan archivos objeto (`.o`). Para eliminar estos archivos y el ejecutable se utiliza:

```bash
make clean
```

El proyecto se compila utilizando las siguientes opciones:

| Opción | Descripción |
|:---|:---|
| `-std=c++17` | Utiliza el estándar C++17. |
| `-Wall` | Habilita las advertencias comunes del compilador. |
| `-Wextra` | Habilita advertencias adicionales. |
| `-Iinclude` | Indica la ubicación de los archivos de cabecera. |
| `-lpthread` | Enlaza la biblioteca solicitada en la pauta de compilación. |

> [!NOTE]
> La presencia de `-lpthread` en el comando de compilación no implica el uso de threads en la solución.

---

## 5. Ejecución

Una vez compilado, el programa se ejecuta desde la carpeta raíz con la siguiente estructura:

```bash
./planificador <archivo.txt> <K>
```

| Parámetro | Descripción |
|:---|:---|
| `<archivo.txt>` | Archivo que contiene la planificación de actividades. |
| `<K>` | Cantidad máxima de procesos que pueden ejecutarse simultáneamente. Debe ser mayor que cero. |

### Ejemplo

```bash
./planificador prueba.txt 2
```

En este ejemplo, el programa utiliza `prueba.txt` como archivo de planificación y permite un máximo de dos procesos concurrentes.

### Salida en terminal

La salida utiliza etiquetas y colores ANSI para distinguir los eventos principales de la planificación:

| Etiqueta | Color | Significado |
|:---|:---|:---|
| `[INICIO]` | Cian | Una actividad comenzó su ejecución. |
| `[FIN]` | Verde | Una actividad terminó correctamente. |
| `[PIPE]` | Amarillo | Se envió o recibió información mediante un pipe. |
| `[INTERRUPCION]` | Azul | Se recibió `SIGINT` mediante `Ctrl+C`. |
| `[FALLIDA]` | Rojo | Una actividad terminó de forma incorrecta. |
| `[ABORTADA]` | Magenta | Una actividad fue abortada por una interrupción o por depender de una actividad fallida. |
| `[ERROR]` | Rojo | Se produjo un error durante la ejecución. |

Las etiquetas permiten reconocer cada evento incluso cuando la configuración de la terminal no diferencia claramente los colores.

---

## 6. Formato del archivo de entrada

Cada línea del archivo representa una actividad y utiliza el siguiente formato:

```text
ID_Actividad : Nombre_Actividad : tiempo_ms : Dependencia1, Dependencia2, ...
```

### Ejemplo

```text
1 : prender_carbon : 500 :
2 : comprar_carne : 1200 :
3 : comprar_pan : 300 :
4 : asar_longaniza : 800 : 1, 2
5 : armar_choripan : 250 : 3, 4
6 : servir_mesa : 100 : 5
```

Las actividades sin dependencias dejan vacío el último campo. Si no se especifica una duración, el programa asigna aleatoriamente un valor entre **100 y 5000 ms**.

---

## 7. Decisiones de diseño

### Organización modular

El proyecto se divide en archivos fuente y de cabecera para mantener separadas sus responsabilidades. De esta manera, `main.cpp` coordina la ejecución general, mientras los demás módulos se encargan del procesamiento del archivo de entrada, la validación del grafo, la planificación de procesos y la presentación de mensajes en la terminal.

### Planificación basada en procesos

Cada actividad se ejecuta mediante un proceso hijo creado con `fork()`. El proceso padre controla la cantidad de actividades activas y respeta el límite de concurrencia indicado por `K`. La espera de procesos se realiza mediante `waitpid()`, evitando la espera activa.

### Comunicación mediante pipes

Los pipes permiten comunicar la finalización de las actividades. El mensaje recibido se almacena para entregarlo posteriormente a las actividades que dependen de ese resultado.

### Manejo de fallos

El proceso padre revisa el estado de término de cada proceso hijo. Si una actividad falla, se marca con el estado `FALLIDA` y se abortan únicamente sus dependientes directos e indirectos. Las ramas independientes continúan su ejecución normalmente.

### Manejo de SIGINT

La señal `SIGINT` permite interrumpir la planificación mediante `Ctrl+C`. Cuando se recibe esta señal, el proceso padre termina y espera a los hijos activos, cierra los pipes pendientes y actualiza el estado de las actividades que no alcanzaron a finalizar.

### Uso de GNU Make

Se utiliza un archivo `Makefile` para centralizar las opciones de compilación y construir todos los módulos mediante un único comando. Esto evita repetir manualmente instrucciones extensas y permite eliminar los archivos generados mediante `make clean`.

--- 

## 8. Pruebas de carga

El planificador puede ser evaluado con una planificación de hasta **10.000 actividades**, correspondiente a una carga máxima estipulada por las condiciones del proyecto. Adicionalmente, se realizó una prueba con **20.000 actividades** para observar su comportamiento por encima del requisito solicitado. Ambas ejecuciones finalizaron correctamente con código de salida `0`, lo que comprueba que el ejecutable es capaz de soportar esa cantidad.

> [!NOTE]
> Debido a la cantidad de mensajes generados, la salida se redirigió a archivos de texto para evitar saturar la terminal. Los archivos utilizados durante estas pruebas no forman parte del repositorio.

---

## 9. Integrantes

- **Gabriela Esparza**
- **Jose Larrondo**

---

<div align="center">

Desarrollado para la asignatura de **Sistemas Operativos** · 2026

</div>