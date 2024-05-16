#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
#include "customqueue.h"


/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    int rank = *((int*)(ptr));
    MPI_Status status;
    //int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        //Update clock
        pthread_mutex_lock(&clock_mutex);
        if(pakiet.ts > LamportClock)
        {
            LamportClock = pakiet.ts+1;
        }
        else
        {
            LamportClock++;
        }
        pthread_mutex_unlock(&clock_mutex);

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
         //prey_not_responded = 1;
         break;
        case KILL_CONFIRMED:
        // nie wiem, czy to ma dostęp do tych zmiennych, trzeba to sprawdzić
        //my_result = 1;
        //prey_not_responded = 1;
            changeState(Killer_won);
        break;
        case PARTNER_REQ:
            //If someone wants to pair allow them
            insert(pairing_queue, pakiet);
            packet_t ans[1];
            ans->src = rank;
            ans->ts = LamportClock;
            pthread_mutex_lock(&stateMut);
            if(stan == Waiting_for_partner)
            {
                sendPacket(ans, pakiet.src, PARTNER_REQ);
                stan = Partner_requested;
            }
            else
            {
                sendPacket(ans, pakiet.src, PARTNER_REQ);
            }
            pthread_mutex_unlock(&stateMut);
            sendPacket(ans, pakiet.src, PARTNER_ACC);
            break;
        case PARTNER_ACC:
            // Increment ACK counter
            break;
        case PISTOL_REQ:
            // nie wiem, czy to ma dostęp do tych zmiennych, trzeba to sprawdzić
            //todo
            //pthread_mutex_lock( &pistol_mutex ); -- niepotrzebne!
            if(stan!= Pistol_Requested && stan != Shooting) {
                // zaakceptuj, że ktoś bierze
                packet_t *res = malloc(sizeof(packet_t));
                res->data = 0;
                sendPacket(res, pakiet.src, PISTOL_ACC);
                //pthread_mutex_unlock(&pistol_mutex);
            }
            else{
                //kolejka_do_odpowiedzi_na_pistolet[ile_requestow_po_pistolet] = pakiet.src;
                //ile_requestow_po_pistolet += 1;
            }
            break;
        case PISTOL_ACC:
                //todo

                break;
	    default:
            debug("Nieznany typ wiadomosci! %d , %d, %d ",pakiet.src, pakiet.data, status.MPI_TAG);
	    break;
        }
    }
    return NULL;
}
