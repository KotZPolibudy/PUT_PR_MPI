#include "main.h"
#include "watek_glowny.h"
#include "util.h"
#include "customqueue.h"

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
int ile_requestow_po_pistolet = 0;
int* kolejka_do_odpowiedzi_na_pistolet;

// Paring
int waiting = -1;
int received_friendship_response = 1; // start with yourself!

// Pistols
int pistolREQ_res = 1; // start with yourself!


// Need someone to kill or get killed
void want_partner() {
    //Check if we aren't already assigned
    pthread_mutex_lock(&stateMut);
    if(stan == Waiting_for_partner)
    {
        pthread_mutex_unlock(&stateMut);
        //Send requests for others
        packet_t* requestRole;
        myrole = -1;
        ACKcount = 0;
        pthread_mutex_lock(&stateMut);
        stan = Partner_requested;
        broadcast(pairing_queue, requestRole, PARTNER_REQ);
        pthread_mutex_unlock(&stateMut);
    }
    
    //wait for getting role assigned
    while(myrole == -1);
    if(myrole == KILLER) {
        // Selected partner - time to go killing
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a killer \n", LamportClock,
               rank, iteration, partnerID);
    }
    else{
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a runner \n", LamportClock,
               rank, iteration, partnerID);
    }
}


void release_pistol(){
    changeState(Killer); //od teraz odpowiadaj pozytywnie na Requesty i odpowiedz na wszystkie "pending" requesty
    for(int i = 0; i < ile_requestow_po_pistolet; i++){
        packet_t *res = malloc(sizeof(packet_t));
        res->data = 0;
        sendPacket(res, kolejka_do_odpowiedzi_na_pistolet[i], PISTOL_ACC); //do kazdego w kolejce
    }

};


void get_pistol(){
    changeState(Pistol_Requested);
    packet_t *reqqq = malloc(sizeof(packet_t));
    reqqq->data = 0;
    broadcast2(reqqq, PISTOL_REQ); //todo broadcast dostał kolejke, trzeba by coś podać w ten argument
    while(pistolREQ_res < size - P){ //todo check the math, czy nie jakies +1 albo -1
        usleep(1000);
    }

}


void try_killing(){
    if(shots_fired < A) {
        get_pistol();
        changeState(Shooting);
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

void do_nothing_basically(){
    //Be a runner, change state?
    usleep(100);
}


void mainLoop()
{
    srandom(rank);
    int tag;
    pairing_queue = create_queue();
    //MPI_Comm_size(MPI_COMM_WORLD, &size);
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank); // chyba potrzebne :D
    // int kolejka_do_odpowiedzi_na_pistolet[size]; // ta linijka jest straszna i należy się jej pozbyć, zamienić na odpowiedniego malloca bo jak to zadziała to tylko przypadkiem
    kolejka_do_odpowiedzi_na_pistolet = (int*)malloc(size*sizeof(int));
    for (int i = 0; i < size; i++) {kolejka_do_odpowiedzi_na_pistolet[i] = -1;} //Fill this with "-1"

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
        printf("[%05d][%02d] -- CODE RUN -- ITERATION %02d --\n", LamportClock, rank, iteration);
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
                do_nothing_basically();
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
    free(kolejka_do_odpowiedzi_na_pistolet);
}
