#include "main.h"
#include "watek_glowny.h"

// My clock value
int lamport_clock = 0;
// Number of pistols
int P = 5;
// Number of cycles
int C = 3;
// Number of ammo
int A = 3;
// My process ID - assigned by MPI
// int myPID;  //rank
// Total process inside world - assigned by MPI
// int total_process;  //size

// Vector with requests - requests to access to find partner critical section
//std::vector<Message> partner_queue;

// Partner ID, default -1 to correct check this inside functions
int partnerID = -1;
//reszta
int myrole = -1;
int iteration = 0;
int my_result;
int result;
int shots_fired = 0;
int prey_not_responded = 1;


// Paring
int waiting = -1;
int received_friendship_response = 1; // start with yourself!

// Pistols
int pistolREQ_res = 1; // start with yourself!


// Need someone to kill or get killed
void want_partner() {
    //Send requests for others
    for(int i = 0; i < size; i++)
    {
        if (i!=rank)
        {
            sendPacket( pkt, i, REQUEST);
        }
    }

    if(myrole == KILLER) {
        // Selected partner - time to go killing
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a killer \n", lamport_clock,
               rank, iteration, partnerID);
    }
    else{
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a runner \n", lamport_clock,
               rank, iteration, partnerID);
    }
}


void Release_pistol(){
    changeState(Killer); //od teraz odpowiadaj pozytywnie na Requesty i odpowiedz na wszystkie "pending" requesty
    for(int i = 0; i < ile_requestow_po_pistolet; i++){
        packet_t *res = malloc(sizeof(packet_t));
        res->data = 0;
        sendPacket(res, kolejka_do_odpowiedzi_na_pistolet[i], PISTOL_ACC); //do kazdego w kolejce
    }

};


void get_pistol(){
    changeState(Pistol_Requested);
    //todo broadcast request
    packet_t req;
    packet_t *req = malloc(sizeof(packet_t));
    req -> data = 0;
    broadcast(req, pistol_REQ);
    while(pistolREQ_res < size - P){ //todo check the math, czy nie jakies +1 albo -1
        usleep(1000);
    }

}


void try_killing(){
    if(shots_fired < A) {
        //todo
        get_pistol();
        changeState(Shooting);
        //endtodo
        shots_fired += 1;
        prey_not_responded = 1;
        int attack = rand() % 20;
        packet_t *pkt = malloc(sizeof(packet_t));
        pkt->data = attack;
        sendPacket(pkt, partnerID, KILL_ATTEMPT);
        //wait for confirmation
        while(prey_not_responded){
            usleep(1000);
        }
        //got confirm, exiting,leaving the pistol
        //todo
        release_pistol();
        changeState(Killer);

    }
    else{
        //send Finish, as there is no more ammo
        packet_t *pkt = malloc(sizeof(packet_t));
        pkt->data = 0;
        sendPacket(pkt, partnerID, FINISH);
    }

}


void mainLoop()
{
    srandom(rank);
    int tag;
    int Vector[size];
    int kolejka_do_odpowiedzi_na_pistolet[size] = {-1};
    for (int i = 0; i < size; i++) {array[i] = -1;} //Fill this with "-1"
    int ile_requestow_po_pistolet = 0;

    while (stan != InFinish) {
        /*
         * //Danilecki template
        int perc = random()%100; 

        if (perc<STATE_CHANGE_PROB) {
            if (stan==InRun) {
		debug("Zmieniam stan na wysyłanie");
		changeState( InSend );
		packet_t *pkt = malloc(sizeof(packet_t));
		pkt->data = perc;
		perc = random()%100;
		tag = ( perc < 25 ) ? FINISH : APP_PKT;
		debug("Perc: %d", perc);
		
		sendPacket( pkt, (rank+1)%size, tag);
		changeState( InRun );
		debug("Skończyłem wysyłać");
            } else {
            }
        }
        sleep(SEC_IN_STATE);
         */

        iteration++;
        printf("[%05d][%02d] -- CODE RUN -- ITERATION %02d --\n", lamport_clock, rank, iteration);

        for (int current_cycle = 0; current_cycle < C; current_cycle++)
        {
            //Refresh data
            myrole = -1;
            changeState(Waiting_for_partner);

            //Find partner
            want_partner();
            //killers kill, runners "run"
            if(myrole == KILLER){
                changeState(Killer);
                try_killing();
            }
            else{
                changeState(Runner);
                do_nothing_basically(); // todo -wait to be killed, or just skip?
            }
            //Results
            if(myrole == KILLER){
                //todo
                //podsumowanie - dopisz wynik do jakiejś tablicy, żeby było na podsumowanie cyklu
            }

            changeState(Finished);
            sleep(SEC_IN_STATE);
        }
        // podsumowanie cyklu
    }
}
