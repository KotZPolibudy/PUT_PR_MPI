#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"
#include "customqueue.h"

int rank, size; // rank == MyPID , size == WORLD_SIZE
int LamportClock = 0;
state_t stan=InRun;
pthread_t threadKom, threadMon;
// Mutex - change state
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
// Mutex - clock
pthread_mutex_t clock_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex - partner queue
pthread_mutex_t partner_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex - pistol queue
pthread_mutex_t pistol_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex - ACK Count
pthread_mutex_t ACK_mutex = PTHREAD_MUTEX_INITIALIZER;
int ACKcount = 0;

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    pthread_mutex_destroy( &clock_mutex);
    pthread_mutex_destroy( &partner_mutex);
    pthread_mutex_destroy( &pistol_mutex);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    pthread_join(threadKom,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* Tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}


int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    inicjuj_typ_pakietu(); // tworzy typ pakietu
    packet_t pkt;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pthread_create( &threadKom, NULL, startKomWatek , 0);

    mainLoop();
    
    finalizuj();
    return 0;
}

