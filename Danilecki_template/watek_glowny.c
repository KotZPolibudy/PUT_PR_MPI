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
int my_ammo;


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
            //todo
            // change state na inRun

            //Find partner
            want_partner();
            //killers kill, runners "run"
            if(myrole == KILLER){
                try_killing();
            }
            else{
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

/*
void mainloop{
        srandom(rank);
        int tag;
        while (stan != InFinish) {
            iteration++;
            printf("[%05d][%02d] -- CODE RUN -- ITERATION %02d --\n", lamport_clock, myPID, iteration);

            for (int current_cycle = 0; current_cycle < C; current_cycle++) {
                // 1. Init variables
                init_variables();

                // 2. Find partner and get roles
                want_partner();   // if left in this state, should be in 1st place for next cycle

                // 3. do your task
                if (killer) {
                    // todo -> name it try_killing() or do_killer_stuff()
                    for (int my_ammo = A; my_ammo > 0 && !killed; my_ammo--) {
                        killed = try_killing(); // todo
                    }

                } else {
                    do_nothing_basically(); // todo - if needed?
                }
                //todo end_cycle(); (if killer only?)
            } // end a cycle
        }  // end while true
};
*/
