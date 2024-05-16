#include "main.h"
#include "util.h"
#include "customqueue.h"
MPI_Datatype MPI_PAKIET_T;

struct tagNames_t{
    const char *name;
    int tag;
} tagNames[] = { { "pakiet aplikacyjny", APP_PKT }, { "finish", FINISH}};

const char *tag2string( int tag )
{
    for (int i=0; i <sizeof(tagNames)/sizeof(struct tagNames_t);i++) {
	if ( tagNames[i].tag == tag )  return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
*/
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int       blocklengths[NITEMS] = {1,1,1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet_t, ts);
    offsets[1] = offsetof(packet_t, src);
    offsets[2] = offsetof(packet_t, data);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t *pkt, int destination, int tag)
{
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->src = rank;
    pthread_mutex_lock(&clock_mutex);
    LamportClock++;
    pkt->ts = LamportClock;
    pthread_mutex_unlock(&clock_mutex);
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d\n", tag2string( tag), destination);
    if (freepkt) free(pkt);
}

void changeState( state_t newState )
{
    pthread_mutex_lock( &state_mutex );
    if (stan==InFinish) { 
	pthread_mutex_unlock( &state_mutex );
        return;
    }
    stan = newState;
    pthread_mutex_unlock( &state_mutex );
}

void tick_Lamport_clock(int new)
{ //default na 0, bo przy wysylaniu np. nie ma nowej wartosci
    pthread_mutex_lock( &clock_mutex );
    if(new > LamportClock){
        LamportClock = new +1;
    }
    else{
        LamportClock += 1;
    }
    pthread_mutex_unlock( &clock_mutex );

}

void broadcast(Queue* q, packet_t *pkt, int tag)
    {
        int freepkt=1;
        if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
        pkt->src = rank;
        pthread_mutex_lock(&clock_mutex);
        LamportClock++;
        pkt->ts = LamportClock;
        pthread_mutex_unlock(&clock_mutex);
        insert(q, *pkt); //sprawdzic czy faktycznie cos wstawia
        for(int j=0; j<size; j++){
            if(rank != j){
                MPI_Send( pkt, 1, MPI_PAKIET_T, j, tag, MPI_COMM_WORLD);
            }
        }
        debug("Wysyłam Grupowo %s\n", tag2string( tag));
        if (freepkt) free(pkt);
    }

void broadcast2(packet_t *pkt, int tag)
{
    int freepkt=1;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->src = rank;
    pthread_mutex_lock(&clock_mutex);
    LamportClock++;
    pkt->ts = LamportClock;
    pthread_mutex_unlock(&clock_mutex);
    for(int j=0; j<size; j++){
        if(rank != j){
            MPI_Send( pkt, 1, MPI_PAKIET_T, j, tag, MPI_COMM_WORLD);
        }
    }
    debug("Wysyłam Grupowo %s\n", tag2string( tag));
    if (freepkt) free(pkt);
}

