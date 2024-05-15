#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch ( status.MPI_TAG ) {
	    case FINISH: 
                changeState(InFinish);
	    break;
	    case APP_PKT: 
                debug("Dostałem pakiet od %d z danymi %d",pakiet.src, pakiet.data);
	    break;
        case KILL_ATTEMPT:
            //todo
            // wylosuj obrone
            // if defend > pakiet.data
            // MPI_Send "KILL_AVOIDED"
            // else MPI_Send "KILL_CONFIRMED" + change state na finished
        break;
        case KILL_AVOIDED:
         //todo;
         break;
        case KILL_CONFIRMED:
            my_result = 1;
            break;
	    default:
	    break;
        }
    }
}
