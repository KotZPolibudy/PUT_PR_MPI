#ifndef UTILH
#define UTILH
#include "packet.h"
#include "customqueue.h"
#include "main.h"


#define KILLER 1
#define RUNNER 0

/* Typy wiadomości */ // TAG, TAGS, tag, Tag, Tags, tags
#define APP_PKT 1
#define FINISH 2
#define KILL_ATTEMPT 11
#define KILL_AVOIDED 12
#define KILL_CONFIRMED 13
#define PARTNER_REQ 20
#define PARTNER_ACC 21
#define PISTOL_REQ 33
#define PISTOL_ACC 34

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

/* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza - stwórz pusty pakiet), do kogo, z jakim typem */
void sendPacket(packet_t *pkt, int destination, int tag);

void tick_Lamport_clock();

void broadcast(Queue* q, packet_t *pkt, int tag);

void broadcast2(packet_t *pkt, int tag);

#endif