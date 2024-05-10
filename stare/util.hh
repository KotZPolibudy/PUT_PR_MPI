#ifndef UTIL_H
#define UTIL_H
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <random>
#include <functional>
#include "main.hh"

using namespace std;

#define RED "\033[41m"
#define GRN "\033[42m"
#define YLW "\033[43m"
#define NLC "\033[0m\n"


// SEKCJA: Struktury i Zmienne -> przeniesione do util

//Wiadomości 
enum Tag { REQ, ACC, FIN, NONE };
struct Msg { 
	int timestamp;
	int My_id;
	Tag tag;
	int message;
	};
#define NITEMS 4
// Zmienne:
const int N = 8; // ilość procesów
#define P 5; // ilość pistoletów
#define C 3; // ilość cykli (na epokę, do podsumowania)
#define A 3; // ilość kul (amunicji)

// Współdzielone:
int Waiting = -1;

// Prywatne:

vector<int> WaitQueue[0];  // todo jak działały wektory bo mnie pamięć zawodzi XD
int AckNum;
int Partner;
int Wynik;

// Stany i role
enum State {REST, WAIT, INSECTION};
enum Role {KILLER, RUNNER, WAITING};
State myState = REST; 
Role myRole = WAITING;

// Zegary

int max_rank; //czy będą potrzebne?
int my_rank;
int my_time = 0;

#define debug(format, ...) fprintf(stderr, "%4d P%d " format, my_time, my_rank, ##__VA_ARGS__)

void tick(int new_time = 0) {
	my_time = max(my_time, new_time) + 1;
}


/* Typy wiadomości */ //
#define APP_PKT 1
#define FINISH 2

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendMsg(Msg *pkt, int destination, int tag);
#endif 