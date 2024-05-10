SOURCES=$(wildcard *.cc)
HEADERS=$(SOURCES:.cc=.hh)
FLAGS=-DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpicc $(SOURCES) $(FLAGS) -o main

clear: clean

clean:
	rm -f main a.out

run: main
	mpiexec -np 8 ./main
