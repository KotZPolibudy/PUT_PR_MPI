SOURCES=$(wildcard *.c*)
HEADERS=$(SOURCES:.c=.h)
FLAGS=-DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) $(FLAGS) -o main -Wall

clear: clean

clean:
	rm main a.out

run: main
	mpiexec -oversubscribe -np 4 ./main
