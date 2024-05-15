#include <stdio.h>
#include <stdlib.h>
#include "customqueue.h"

// Function to create an empty queue
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

// Function to create a new node
QueueNode* create_node(packet_t packet) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->packet = packet;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a packet into the queue
void insert(Queue* q, packet_t packet) {
    QueueNode* newNode = create_node(packet);
    if (!q->front || q->front->packet.ts > packet.ts || 
        (q->front->packet.ts == packet.ts && q->front->packet.src > packet.src)) {
        newNode->next = q->front;
        q->front = newNode;
        if (!q->rear) {
            q->rear = newNode;
        }
    } else {
        QueueNode* current = q->front;
        while (current->next && 
               (current->next->packet.ts < packet.ts || 
               (current->next->packet.ts == packet.ts && current->next->packet.src < packet.src))) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
        if (!newNode->next) {
            q->rear = newNode;
        }
    }
}

// Function to delete an element from the queue
packet_t delete(Queue* q) {
    if (!q->front) {
        printf("Queue is empty\n");
        exit(EXIT_FAILURE);
    }
    QueueNode* temp = q->front;
    packet_t packet = temp->packet;
    q->front = q->front->next;
    if (!q->front) {
        q->rear = NULL;
    }
    free(temp);
    return packet;
}

// Function to find the position of an element in the queue
int find_position(Queue* q, packet_t packet) {
    QueueNode* current = q->front;
    int position = 0;
    while (current) {
        if (current->packet.ts == packet.ts && current->packet.src == packet.src) {
            return position;
        }
        current = current->next;
        position++;
    }
    return -1; // Not found
}

// Function to display the queue
void display_queue(Queue* q) {
    QueueNode* current = q->front;
    while (current) {
        printf("Packet ts: %d, src: %d, data: %d\n", current->packet.ts, current->packet.src, current->packet.data);
        current = current->next;
    }
}
