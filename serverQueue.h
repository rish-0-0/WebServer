#ifndef SERVERQUEUE_H_
#define SERVERQUEUE_H_

struct node_t {
	int* client_socket;
	struct node_t* next;
};

typedef struct node_t node;

void enqueue(int* client_socket);
int* dequeue();

#define MAX_QUEUE_LENGTH 31
#endif