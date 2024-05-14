#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <climits>
#include <random>
#include "tags.h"
#include "utils.h"
#include "communication.h"


// ZMIENNE:

// Debug mode - if set `true` - show all logs.
// On `false` show only most important logs
bool debug_mode = false;
// My clock value
int lamport_clock = 0;
// Number of pistols
int P = 5;
// Number of cycles
int C = 3;
// Number of ammo
int A = 3;
// My process ID - assigned by MPI
int myPID;
// Total process inside world - assigned by MPI
int total_process;
// Important parameter to enable correct finish threads - without extra variable can raise errors
bool run_program = true;


// Vector with requests - requests to access to find partner critical section
std::vector<Message> partner_queue;
// Vector with requests to access saloon's critical section
std::vector<Message> saloon_queue;
// List of houseIDs to return
std::vector<int> houses_to_return_list;
// Array with lists of requests to all houses
std::vector<Message> *houses_vec;

// Mutex - clock
pthread_mutex_t clock_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex - partner queue
pthread_mutex_t partner_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex - partner response number (for secure update variable because is it possible to send multi messages)
pthread_mutex_t partner_response_mutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex arrays for houses
std::vector<pthread_mutex_t> houses_mutex;
// Single mutex for access to table
pthread_mutex_t houses_array_mutex = PTHREAD_MUTEX_INITIALIZER;
// Saloon mutex
pthread_mutex_t saloon_mutex = PTHREAD_MUTEX_INITIALIZER;
// Houses to return mutex
pthread_mutex_t houses_to_return_mutex = PTHREAD_MUTEX_INITIALIZER;

// Number of received ACK to friend find critical section
// Default 1 - your own ACK ;)
int received_friendship_response = 1;
int received_saloon_ack = 1;
// Partner ID, default -1 to correct check this inside functions
int partnerID = -1;
// Selected house ID
int houseID = -1;
// Array with responses (counters) about houses
int *houses_responses_array;
// Killer / prey flag
bool killer = false;
// flag if killed already
bool killed = false;
// Iteration counter
int iteration = 0;
int currentActionID = 0;


bool generateRandomBoolean() {
    // Create a random number generator engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1); // Distribution for 0 and 1

    // Generate a random integer (0 or 1)
    int randomInt = dis(gen);

    // Convert the integer to boolean
    return static_cast<bool>(randomInt);
}


bool set_parameters(int argc, char *argv[]) {
    // Required arguments -> pistols, cycles, ammo
    if (argc >= 4) {
        P = atoi(argv[1]);
        C = atoi(argv[2]);
        A = atoi(argv[2]);
        // Validate positive value inside variable
        if (P > 0 && C > 0 && A > 0) {
            return true;
        }
    }

    // Any error - false (error found)
    return false;
}

void enable_thread(int *argc, char ***argv) {
    // Check support
    int status = 0;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &status);

    // Debug mode? - Show current MPI support level
    if (debug_mode) {
        switch (status) {
            case MPI_THREAD_SINGLE:
                printf("[INFO] Thread level supported: MPI_THREAD_SINGLE\n");
                break;
            case MPI_THREAD_FUNNELED:
                printf("[INFO] Thread level supported: MPI_THREAD_FUNNELED\n");
                break;
            case MPI_THREAD_SERIALIZED:
                printf("[INFO] Thread level supported: MPI_THREAD_SERIALIZED\n");
                break;
            case MPI_THREAD_MULTIPLE:
                printf("[INFO] Thread level supported: MPI_THREAD_MULTIPLE\n");
                break;
            default:
                printf("[INFO] Thread level supported: UNRECOGNIZED\n");
                exit(EXIT_FAILURE);
        }
    }

    // When thread not supported - exit
    if (status != MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "[ERROR] There is not enough support for threads - I'm leaving!\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
}
// receiving thread loop
void *receive_loop(void *thread) {
    // Run in loop until `run_program` set as true
    while(run_program) {
        // Status & data to receive function
        MPI_Status status;
        int data[3];
        // Receive message
        receive(lamport_clock, data, status, MPI_ANY_TAG, myPID, MPI_ANY_SOURCE);

        // Check status and do code
        switch (status.MPI_TAG) {
            case TAG_FIND_PARTNER: {
                // Append request with original time to queue
                // todo
                insert_partner_request(data[2], status.MPI_SOURCE);
                // Send response - note sender about receive message
                send(lamport_clock, iteration, myPID, TAG_RESPONSE_PARTNER, status.MPI_SOURCE, myPID);

                // End case TAG_FIND_PARTNER
                break;
            }

            case TAG_RESPONSE_PARTNER: {
                // Increment received response counter - for security with mutex
                // todo
                pthread_mutex_lock(&partner_response_mutex);
                received_friendship_response++;
                pthread_mutex_unlock(&partner_response_mutex);

                // End case TAG_RESPONSE_PARTNER
                break;
            }

            case TAG_SELECTED_PARTNER: {
                // I was chosen!
                // todo

                // End case TAG_SELECTED_PARTNER
                break;
            }



            default: {
                // Default - raise error because received not supported TAG inside message
                printf("[%05d][%02d][ERROR] Invalid tag '%d' from process %d.\n", lamport_clock, myPID, status.MPI_TAG, status.MPI_SOURCE);
                exit(1);
            }
        }
    }

    return 0;
}


// void *release_assigned_houses(void *thread) {
void *Send_thread_from_queue(void *thread) {
    //todo
    get_sending_queue();
    send_the_thing();
    return nullptr;
}


int check_position(pthread_mutex_t &mutex, std::vector<Message> &list, int pid) {
    int position = INT_MAX;
    pthread_mutex_lock(&mutex);
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].pid == pid) {
            position = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return position;
}


void init_variables(){
    // todo
}

// WAŻNE! MUSI BYĆ OBŁOŻONE MUTEXAMI
void remove_from_friendship_queue(int senderID) {
    for (size_t i = 0; i < partner_queue.size(); i++) {
        if (partner_queue[i].pid == senderID) {
            partner_queue.erase(partner_queue.begin() + i);
            break;
        }
    }
}


// Need someone to kill or get killed
void want_partner() {
    Message temp = Message(lamport_clock, myPID);
    // Dopisz się do kolejki
    // Lock, append request, sort, unlock
    pthread_mutex_lock(&partner_mutex);
    partner_queue.push_back(temp);
    received_friendship_response = 1; // start with 1 = YOU
    pthread_mutex_unlock(&partner_mutex);

    // Broadcast find partner request
    broadcast(lamport_clock, iteration, temp.time, TAG_FIND_PARTNER, total_process, myPID);
    // Wait until receive all confirmations
    while(received_friendship_response < total_process) {
        usleep(1000);
    }

    printf("[%05d][%02d] Received all messages\n", lamport_clock, myPID);

    int my_pos;
    my_pos = check_position(partner_mutex, partner_queue, myPID);
    // did I get my role yet?
    bool didnt_get_role_yet = true;
    if(my_pos % 2 == 1){
        didnt_get_role_yet = false;
        pthread_mutex_lock(&partner_mutex);
        partnerID = partner_queue[my_pos-1].pid;
        remove_from_friendship_queue(myPID);
        remove_from_friendship_queue(partnerID);
        pthread_mutex_unlock(&partner_mutex);
        killer = generateRandomBoolean();
        //todo send other role to the process number at my_pos-1 in partner_queue
        //delete self and partner from partner queue



    }
    else{
        while(didnt_get_role_yet){
            //wait till you receive your role
            // hopefully you are not stuck here until the next cycle, but maybe you are :D
            usleep(1000);
            //also set the variables of my_role and partnerID <= in comms function
            // lets just swap my_role for a flag "bool killer"
        }
    }

    if(killer) {
        // Selected partner - time to go killing
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a killer \n", lamport_clock,
               myPID, iteration, partnerID);
    }
    else{
        printf("[%05d][PID: %02d][IT: %02d] I have partner! Selected process %02d And I am a runner \n", lamport_clock,
               myPID, iteration, partnerID);
    }
}


void do_nothing_basically(){
    //todo change status to FINISHED
}


int main(int argc, char *argv[]) {
    // stdout - disable the buffer
    setbuf(stdout, NULL);

    //todo - check that
    //MPI_INIT(); ????? - ni było, a chyba działało, więc się dziwie XD

    // Set program parameters
    if (!set_parameters(argc, argv)) {
        puts("[ERROR] You should start with NAME Pistols Cycles Ammo parameters (greater than zero)");
    } else {
        // Parameters setup with success
        if (debug_mode) {
            printf("[INFO] Parameters setup correct\n");
        }

        // Check MPI threads
        enable_thread(&argc, &argv);

        // Get process ID and total process number
        MPI_Comm_rank(MPI_COMM_WORLD, &myPID);
        MPI_Comm_size(MPI_COMM_WORLD, &total_process);

        // Create new thread - run for receive messages in loop (as monitor)
        pthread_t monitor_thread_receiver;
        pthread_create(&monitor_thread_receiver, nullptr, receive_loop, 0);
        pthread_t monitor_thread_sender;
        pthread_create(&monitor_thread_sender, nullptr, Send_thread_from_queue, 0);


        // Random seed depends on process myPID
        srand(myPID);

        // Initialize variables

        // Barier to start calculations
        if (debug_mode) {
            printf("[%05d][%02d][INFO] PROCESS %02d READY\n", lamport_clock, myPID, myPID);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        // while(1) {   <-- finalnie to ma być tak
        while (iteration < 5) {
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
    }   //end else in if set parameters
    // Set end calculations
    run_program = false;

    // Sleep to ensure all threads refresh local reference to `run_program` variable
    sleep(10);

    // Finalize MPI
    MPI_Finalize();

    // End without errors
    return 0;

}


