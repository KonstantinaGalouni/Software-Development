#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/******************************************************************************/
void bfs(Queue **queue,Graph *visited1,Graph *visited2,Record **node1,Record **node2,Graph *g,int *min)
{
	Record *r, *temp,*current;
	Edge *e;
	
	current = dequeue(queue);
	
	if(current == NULL){		//empty queue
		(*node1) = NULL;
		return ;
	}	

	r = lookupNode(current->id,g);
	
	if(r == NULL){
		printf("There is no record with id = %d\n", current->id);
		(*node1) = NULL;
		return ;
	}

	e = r->edges;
	
	int d = getIntegerProperty(2, current->props);	//current distance
	
	while(e != NULL){
		//for all edges
		if((r = lookupNode(e->id, visited1)) == NULL){
			//e->id not found in visited1
			
			r = setPersonProperties(e->id, NULL, NULL, d+1);
			//create record with same id as the edge, distance=current distance +1
			
			insertNode(r , visited1);
			enqueue(queue,r);
			
			if((temp = lookupNode(e->id, visited2)) != NULL){
				//e->id found in visited2
				
				int distance = getIntegerProperty(2, temp->props);
				if(*min == -1 || distance < *min){
					//found solution with less distance
					
					(*node2) = temp;
					(*node1) = r;
					*min = distance;
				}
				while(!(empty(*queue)) && getIntegerProperty(2,(*queue)->first->node->props) == d){
					//search all records from queue with the same distance as current record
					bfs(queue,visited1,visited2,node1,node2,g,min);
				}				
			}				
		}
		e = e->next;			//next edge
	}
	
	if((*min) == -1 ){
		(*node1) = current;
	}
	
}
/******************************************************************************/
void create_queue(Queue **q)
{
	(*q) = malloc(sizeof(Queue));
	(*q)->first = NULL;
	(*q)->last  = NULL;
}
/******************************************************************************/
void destroy_queue(Queue **q)
{	
	while(!empty(*q)){
		dequeue(q);
	}
	free(*q);
	(*q) = NULL;
}
/******************************************************************************/
void enqueue(Queue **q , Record *node)
{	
	Queue_Node *new;
	
	new = malloc(sizeof(Queue_Node));
	new->node = node;
	new->next = NULL;
	
	if(empty(*q)){
		(*q)->first = (*q)->last = new;
	}else{
		(*q)->last->next = new;				//put at the end of the queue q
		(*q)->last = new;					//new record is the last one
	}	
}
/******************************************************************************/
Record* dequeue(Queue **q)
{
	Record *node;	
	Queue_Node *delete;
	
	if(empty(*q)){
		return NULL;
	}
	
	delete = (*q)->first;	
	
	(*q)->first = (*q)->first->next;		//dequeue the first record
	
	if(delete == (*q)->last){
		(*q)->first = (*q)->last = NULL;
	}
	
	node = delete->node;
	free(delete);
	
	return node;	
}
/******************************************************************************/
int empty(Queue *q)
{
	return (q->first == NULL);
}
/******************************************************************************/
