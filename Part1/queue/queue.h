#ifndef QUEUE_H_
#define QUEUE_H_
#include "../graph/graph.h"
#include "../main/main_functions.h"

typedef struct Queue_Node{
	Record *node;
	struct Queue_Node *next;
}Queue_Node;
	
typedef struct Queue{
	Queue_Node *first;
	Queue_Node *last;
}Queue;


void bfs(Queue **queue,Graph *visited1,Graph *visited2,Record **node1,Record **node2,Graph *g,int *min);

void create_queue(Queue **q);
void destroy_queue(Queue **q);
void enqueue(Queue **q , Record *node);
Record* dequeue(Queue **q);
int empty(Queue *q);

#endif
