#ifndef CUSTOMQUEUE_H
#define CUSTOMQUEUE_H
#include "packet.h"
//#include "util.h" // Include util.h to access packet_t

// Queue structure
typedef struct QueueNode {
    packet_t packet;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Function declarations
Queue* create_queue();
void insert(Queue* q, packet_t packet);
void delete_at_position(Queue* q, int position);
void delete_front(Queue* q);
int find_position(Queue* q, packet_t packet);
void display_queue(Queue* q);

#endif /* CUSTOMQUEUE_H */
