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
//Tablica wynikow cykli
//int Res[C];


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
int Wygrane = 0, Przegrane =0;

// Paring
int waiting = -1;
int received_friendship_response = 1; // start with yourself!

// Pistols
int pistolREQ_res = 1; // start with yourself!

Queue* pairing_queue;
Queue* pistol_queue;

// Need someone to kill or get killed
void want_partner()
{
    packet_t* requestRole = malloc(sizeof(packet_t));
    //Check if we haven't already sent a request
    pthread_mutex_lock(&state_mutex);
    if(stan == SupportsPairing)
    {
        pthread_mutex_unlock(&state_mutex);
        pthread_mutex_lock(&ACK_mutex);
        ACKcount = 0;
        myrole = -1;
        pthread_mutex_unlock(&ACK_mutex);
        //Send requests for others
        pthread_mutex_lock(&state_mutex);
        stan = Partner_requested;
        broadcast(requestRole, PARTNER_REQ);
    }
    pthread_mutex_unlock(&state_mutex);
    
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
        sendPacket(res, kolejka_do_odpowiedzi_na_pistolet[i], PISTOL_ACC); //do każdego w kolejce
    }

};


void get_pistol(){
    changeState(Pistol_Requested);
    packet_t *reqqq = malloc(sizeof(packet_t));
    reqqq->data = 0;
    broadcast(reqqq, PISTOL_REQ); // broadcast 2 to dwuargumentowy, bo idk i nie chce wiedzieć co zmieniłeś XD
    while(pistolREQ_res < size - P){ //zaczynamy od 1, bo od siebie, więc nie trzeba +-1 (chyba)
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
    usleep(9000000);
}


void mainLoop()
{
    srandom(rank);
    //int tag;
    pairing_queue = create_queue();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // chyba potrzebne :D
    pthread_mutex_lock(&state_mutex);
    stan = SupportsPairing;
    pthread_mutex_unlock(&state_mutex);
    // int kolejka_do_odpowiedzi_na_pistolet[size]; // ta linijka jest straszna i należy się jej pozbyć, zamienić na odpowiedniego malloc, bo jak to zadziała to tylko przypadkiem
    // kolejka_do_odpowiedzi_na_pistolet = (int*)malloc(size*sizeof(int));
    // sugeruje uzyc mojego
    pistol_queue = create_queue();
    //for (int i = 0; i < size; i++) {kolejka_do_odpowiedzi_na_pistolet[i] = -1;} //Fill this with "-1"
    while(1)
    {
        want_partner();
        //debug("skoczylem");
        //do_nothing_basically();
        //debug("poczekalem");
        //display_queue(pairing_queue);
        //do_nothing_basically();
        //break;
    }
    while(1);
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
            pthread_mutex_lock(&state_mutex);
            changeState(SupportsPairing);
            pthread_mutex_unlock(&state_mutex);

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
                if (stan == Killer_won){
                    //Res[current_cycle] = 1;
                    Wygrane += 1;
                }
                else{
                    //Res[current_cycle] = 0;
                    Przegrane += 1;
                }

            }

            changeState(Finished);
            sleep(SEC_IN_STATE);

        }
        // podsumowanie cyklu
        //todo Printf ile wygranych ile przegranych
    }
    free(kolejka_do_odpowiedzi_na_pistolet);
}
