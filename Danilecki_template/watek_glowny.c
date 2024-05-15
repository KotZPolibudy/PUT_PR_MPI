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


// Need someone to kill or get killed
void want_partner() {
    //todo
    // Broadcast find partner request
    // broadcast(lamport_clock, iteration, temp.time, TAG_FIND_PARTNER, total_process, myPID);

    // Wait until receive all confirmations
    while(received_friendship_response < total_process) {
        usleep(1000);
    }
    printf("[%05d][%02d] Received all messages\n", lamport_clock, myPID);


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

void try_killing(){
    if(shots_fired < A) {
        shots_fired += 1;
        prey_not_responded = 1;
        int attack = random() % 20;
        packet_t *pkt = malloc(sizeof(packet_t));
        pkt->data = attack;
        sendPacket(pkt, partnerID, KILL_ATTEMPT);
        //todo wait for confirmation
        while(prey_not_responded){
            usleep(1000);
        }
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
        printf("[%05d][%02d] -- CODE RUN -- ITERATION %02d --\n", lamport_clock, myPID, iteration);

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
                //change state na finished
                //podsumowanie
            }
            //todo changeState na finished
            sleep(SEC_IN_STATE);
        }
    }
}