#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"

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
        //todo tick lamport_clock? - to zrobię na końcu, bo to wydaje się proste
        //tick_lamport_clock(VALUE)

        switch ( status.MPI_TAG ) {
	    case FINISH: 
                changeState(Finished);
	    break;
	    case APP_PKT: 
                debug("Dostałem pakiet od %d z danymi %d",pakiet.src, pakiet.data);
	    break;
        case KILL_ATTEMPT:
            //todo
            int def = rand()%18;
            packet_t *res = malloc(sizeof(packet_t));
            res -> data = def;
            if (def > pakiet.data){
                sendPacket(res, pakiet.src, KILL_AVOIDED);
            }
            else{
                sendPacket(res, pakiet.src, KILL_AVOIDED);
                changeState(Finished);
            }
        break;
        case KILL_AVOIDED:
         // nie wiem, czy to ma dostęp do tych zmiennych, trzeba to sprawdzić
         prey_not_responded = 1;
         break;
        case KILL_CONFIRMED:
            // nie wiem, czy to ma dostęp do tych zmiennych, trzeba to sprawdzić
            my_result = 1;
            prey_not_responded = 1;
            break;
            case PARTNER_REQ:
                //todo
                break;
            case PARTNER_ACC:
                //todo
                break;
            case PISTOL_REQ:
                // nie wiem, czy to ma dostęp do tych zmiennych, trzeba to sprawdzić
                packet_t *res = malloc(sizeof(packet_t));
                res -> data = 0;
                pthread_mutex_lock( &pistol_mutex );
                sendPacket(res, pakiet.src, PISTOL_ACC);
                pthread_mutex_unlock( &pistol_mutex );
                break;
            case PISTOL_ACC:


                break;
	    default:
            debug("Nieznany typ wiadomosci! %d , %d, %d ",pakiet.src, pakiet.data, status.MPI_TAG);
	    break;
        }
    }
}
