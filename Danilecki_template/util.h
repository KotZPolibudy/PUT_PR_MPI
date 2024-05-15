#ifndef UTILH
#define UTILH
#include "main.h"


#define KILLER 1
#define RUNNER 0




/* typ pakietu */
typedef struct {
    int ts;       /* timestamp (zegar lamporta */
    int src;  

    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
} packet_t;
/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w inicjuj_typ_pakietu */
#define NITEMS 3

/* Typy wiadomości */
#define APP_PKT 1
#define FINISH 2
#define KILL_ATTEMPT 11
#define KILL_AVOIDED 12
#define KILL_CONFIRMED 13
#define PARTNER_REQ 20
#define PARTNER_ACC 21

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);

void tick_Lamport_clock();
#endif
