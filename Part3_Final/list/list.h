#ifndef QUEUE_H_
#define QUEUE_H_
#include "../graph/graph.h"
#include "../main/main_functions.h"
#include "../metrics/metrics.h"

typedef struct List_Node{
	void *node;
	struct List_Node *next;
}List_Node;
	
typedef struct List{
	List_Node *first;
	List_Node *last;
	//int size;
}List;

void bfs(List **queue,Graph *visited1,Graph *visited2,Record **node1,Record **node2,Graph *g,int *min);
void transfer(Graph *source,Graph *target);
void create_list(List **l);
void destroy_list(List **l);
void enqueue(List **q ,void *node);
void* dequeue(List **q);
void push(List **l,void *node);
int empty(List *q);

#endif
