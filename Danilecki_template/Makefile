SOURCES=$(wildcard *.c*)
HEADERS=$(SOURCES:.c=.h)
FLAGS=-DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) $(FLAGS) -o main -Wall -g

clear: clean

clean:
	rm main a.out

run: main
	mpiexec -oversubscribe -np 8 ./main

run_2: main
	mpiexec -oversubscribe -np 2 ./main
