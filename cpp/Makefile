SOURCES=$(wildcard *.c)
HEADERS=$(SOURCES:.c=.h)
FLAGS=-m 4 -DDEBUG -g

all: main

main: $(SOURCES) $(HEADERS)
	mpiexec $(SOURCES) $(FLAGS) -o main

clear: clean

clean:
	rm main a.out

run: main
	mpiexec -oversubscribe -np 8 ./main
