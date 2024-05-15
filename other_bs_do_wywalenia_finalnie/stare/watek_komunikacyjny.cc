#include "main.hh"
#include "watek_komunikacyjny.hh"
#include "util.hh"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    Tag tag;
    int is_message = FALSE;
    // packet_t pakiet;
    Msg msg;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	debug("czekam na recv");
        MPI_Recv( &msg, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch ( status.MPI_TAG ) {
	    case FINISH: 
                changeState(InFinish);
	    break;
	    case APP_PKT: 
                debug("Dostałem pakiet od %d z danymi %d",pakiet.src, pakiet.data);
	    break;
	    default:
	    break;
        }

    }
}
