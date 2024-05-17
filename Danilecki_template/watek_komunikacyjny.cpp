#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
#include <vector>


int position(std::vector<std::pair<int, int>> queue, int who)
{
	for(int i=0; i<(int)queue.size(); i++)
    {
		if(queue[i].second == who) return i;
	}
	return -1;
}

void displayQUEUE(std::vector<std::pair<int, int>> queue)
{
    debug("moja kolejka");
	for(int i=0; i<(int)queue.size(); i++)
    {
		debug("ts: %d who: %d", queue[i].first, queue[i].second);
	}
    debug("end of queue");
}

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    std::vector<std::pair<int, int>> pairingQueue;
	std::vector<std::pair<int, int>> gunQueue;
    MPI_Status status;
    packet_t pakiet;
    //packet_t *response = (packet_t *)malloc(sizeof(packet_t));
    //response->src = rank;
    //response->data = 0;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (stan != FINISH)
    {
        int placement = 0;
        int defence = 0;
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
                changeState(FINISHED);
	    break;
        case PARTNER_REQ: 
			//debug("Dostałem PARTNER_REQ od %d", status.MPI_SOURCE);
			sendPacket( 0, status.MPI_SOURCE, PAIRING_ACK);
			while(placement < pairingQueue.size())
            {
				if(pairingQueue[placement].first > pakiet.ts ||
                (pairingQueue[placement].first == pakiet.ts && pairingQueue[placement].second > status.MPI_SOURCE))
					break;
                placement++;
            }
			pairingQueue.insert(pairingQueue.begin() + placement, std::make_pair(pakiet.ts, status.MPI_SOURCE));
		    break;
        case PAIRING_ACK: 
			//debug("Dostałem PAIRING_ACK od %d", status.MPI_SOURCE);
            pthread_mutex_lock(&ACK_mutex);
            ACKcount++;
            //debug("ile mi zaakceptowalo %d", ACKcount);
            if(ACKcount == size - 1)
            {
                ACKcount = -1;
                placement = position(pairingQueue, rank);
                if(placement % 2 == 1)
                {
                    myrole = KILLER;
                    partnerID = pairingQueue[placement - 1].second;
                    //response->data = partnerID;
                    //Pair runner and killer
                    sendPacket(0, partnerID, YOU_ARE_RUNNER);
                }
                else
                {
                    haveme = true;
                    debug("completed");
                }
            }
            pthread_mutex_unlock(&ACK_mutex);
	        break;
        case YOU_ARE_RUNNER:
            myrole = RUNNER;
            partnerID = pakiet.src;
            if(haveme == false)
            {
                sendPacket(0, partnerID, WAIT);
            }
            else
            {
                haveme = false;
                sendPacket(0, partnerID, YOU_ARE_KILLER);
            }
            break;
        case YOU_ARE_KILLER: //Probably never used, since killer initializes pairing
            myrole = KILLER;
            partnerID = pakiet.src;
            broadcast(0, REMOVE_FROM_PAIRING_QUEUE);
            break;
        case WAIT:
            sendPacket(0, partnerID, YOU_ARE_RUNNER);
            break;
        case REMOVE_FROM_PAIRING_QUEUE:
            debug("%d prosi o usuniecie z kolejki siebie i jeszcze partnera %d", pakiet.src, pakiet.data);
            displayQUEUE(pairingQueue);
            placement = position(pairingQueue, pakiet.src);
            pairingQueue.erase(pairingQueue.begin() + placement - 1, pairingQueue.begin() + placement + 1);
            debug("PO USUNIECIU");
            displayQUEUE(pairingQueue);
            break;
        case GUN_REQ:
            //debug("Dostałem GUN_REQ od %d", status.MPI_SOURCE);
			sendPacket( 0, status.MPI_SOURCE, PAIRING_ACK);
			while(placement < gunQueue.size())
            {
				if(gunQueue[placement].first > pakiet.ts ||
                (gunQueue[placement].first == pakiet.ts && gunQueue[placement].second > status.MPI_SOURCE))
					break;
                placement++;
            }
			gunQueue.insert(gunQueue.begin() + placement, std::make_pair(pakiet.ts, status.MPI_SOURCE));
		    break;
        case GUN_ACK:
            //debug("Dostałem GUN_ACK od %d", status.MPI_SOURCE);
            pthread_mutex_lock(&ACK_mutex);
            ACKcount++;
            //debug("ile mi zaakceptowalo %d", ACKcount);
            if(ACKcount == size - 1)
            {
                ACKcount = 0;
                placement = position(pairingQueue, rank);
                if(placement % 2 == 1)
                {
                    myrole = KILLER;
                    partnerID = pairingQueue[placement - 1].second;
                    //response->data = partnerID;
                    //Pair runner and killer
                    sendPacket(0, partnerID, YOU_ARE_RUNNER);
                    broadcast(0, REMOVE_FROM_PAIRING_QUEUE);
                }
            }
            pthread_mutex_unlock(&ACK_mutex);
	        break;
        case KILL_ATTEMPT:
            defence = rand()%20;
            if (defence > pakiet.data)
            {
                score++;
                sendPacket(0, pakiet.src, KILL_AVOIDED);
            }
            else
            {
                score--;
                sendPacket(0, pakiet.src, KILL_CONFIRMED);
            }
            roundsfinished++;
        break;
        case KILL_AVOIDED:
            score--;
            roundsfinished++;
        break;
        case KILL_CONFIRMED:
            score++;
            roundsfinished++;
        break;
	    default:
            debug("Nieznany typ wiadomosci! %d , %d, %d ",pakiet.src, pakiet.data, status.MPI_TAG);
	    break;
        }
    }
    return NULL;
}
