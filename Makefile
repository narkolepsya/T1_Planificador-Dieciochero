# Compilador de C++
CXX = g++

# Carpeta que contiene los archivos de cabecera
INCLUDE = include

# Opciones de compilación
CXXFLAGS = -std=c++17 -Wall -Wextra -I$(INCLUDE)

# Librerías
LDLIBS = -lpthread

# Nombre del ejecutable
TARGET = planificador

# Archivos fuente del proyecto
SRCS = src/main.cpp \
       src/parser.cpp \
       src/grafo.cpp \
       src/scheduler.cpp \
       src/terminal.cpp

# Archivos objeto
OBJS = $(SRCS:.cpp=.o)

# Regla principal
all: $(TARGET)

# Enlaza los archivos objeto
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

# Compila cada archivo fuente
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Elimina archivos generados
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean