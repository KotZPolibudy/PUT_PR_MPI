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
    packet_t *response = malloc(sizeof(packet_t));
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish )
    {
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
        response->src = rank;
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
        debug("Dostalem pytanie o partnera od %d", pakiet.src);
            //If someone wants to pair allow them and place them in the queue
            pthread_mutex_lock(&queue_mutex);
            insert(pairing_queue, pakiet);
            pthread_mutex_unlock(&queue_mutex);
            response->ts = LamportClock;
            pthread_mutex_lock(&state_mutex);
            if(stan == SupportsPairing)
            {
                sendPacket(response, pakiet.src, PARTNER_REQ);
                stan = Partner_requested;
            }
            pthread_mutex_unlock(&state_mutex);
            sendPacket(response, pakiet.src, PAIRING_ACK);
            break;
        case PAIRING_ACK:
        debug("Dostalem potwierdzenie na pytanie o partnera od %d", pakiet.src);
            // Increment ACK counter
            pthread_mutex_lock(&ACK_mutex);
            ACKcount++;
            //Check how many responses we have
            if(ACKcount == size - 1)
            {
                pthread_mutex_unlock(&ACK_mutex);
                int position = find_position(pairing_queue, *response);
                //odd-placed processes inform their partners
                if(position % 2 == 1)
                {
                    myrole = KILLER;
                }
                partnerID = get_nth_element(pairing_queue, position-1);
                //Remove both from queue
                //Or not
                sendPacket(response, partnerID, YOU_ARE_RUNNER);
                //
            }
            else
            {
                pthread_mutex_unlock(&ACK_mutex);
            }
            break;
        case YOU_ARE_RUNNER:
            myrole = RUNNER;
            partnerID = pakiet.src;
            break;
        case YOU_ARE_KILLER: //Probably never used, since killer initializes pairing
            myrole = KILLER;
            partnerID = pakiet.src;
            break;
        case REMOVE_FROM_QUEUE: //Probably never used, since killer initializes pairing
            myrole = KILLER;
            partnerID = pakiet.src;
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
