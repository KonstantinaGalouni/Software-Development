#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/******************************************************************************/
int reachNode1_simpeBFS(int start, int end, Graph *g)
{
	int curDist, distance = INFINITY_REACH_NODE;
	
	Graph *visited;
	visited = create_Graph(2,3);
	
	List *queue;	
	create_list(&queue);
	
	Record *r,*node;
	Edge *e;
	Properties *prop;
	
	r = lookupNode(start, g);
	node = lookupNode(end, g);
	
	if(r==NULL){
		printf("There is no record with id = %d\n", start);
		return -1;
	}
	if(node==NULL){
		printf("There is no record with id = %d\n", end);
		return -1;
	}
	
	prop = create_Properties(1);
	setIntegerProperty(0, 0, prop);
	r = create_Node(start, prop);
	insertNode(r , visited);
	enqueue(&queue,r);
	
	while(!empty(queue)){
		node = dequeue(&queue);
		r = lookupNode(node->id,visited);
		
		curDist = getIntegerProperty(0,r->props);		
		
		if(r->id == end){
			distance = getIntegerProperty(0,r->props);	//apostash tou node
			break;
		}
		
		r = lookupNode(node->id, g);
		e = r->edges;
		while(e!=NULL){
			if(lookupNode(e->id, visited) == NULL){
				if(e->id == end){
					distance =  curDist + 1;
					break;
				}
				prop = create_Properties(1);
				setIntegerProperty(curDist + 1, 0, prop);
				r = create_Node(e->id, prop);
				insertNode(r , visited);
				enqueue(&queue,r);
			}
			e = e->next;
		}
		if(distance == curDist+1){
			break;
		}
	}
	
	destroyGraph3(&visited, 1, 0, -1, -1);
	destroy_list(&queue);
	
	return distance;
}
/******************************************************************************/
void bfs(List **queue,Graph *visited1,Graph *visited2,Record **node1,Record **node2,Graph *g,int *min)
{
	Record *r, *temp,*current;
	Edge *e;
	Properties *prop;	
	
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
	
	int d = getIntegerProperty(0, current->props);	//current distance
	int oldMIN = *min;
	
	while(e != NULL){	
	
		if((r = lookupNode(e->id, visited1)) == NULL){	
			
			prop = create_Properties(PERSON_PROPERTIES_NUM);
			setIntegerProperty(d+1, 0, prop);
			r = create_Node(e->id, prop);
			insertNode(r , visited1);
			enqueue(queue,r);
			
			if((temp = lookupNode(e->id, visited2)) != NULL){				
				int distance = getIntegerProperty(0, temp->props);
				if(*min == -1 || (d+distance) <= *min){						//found solution with less or same distance				
					(*node2) = temp;
					(*node1) = r;
					*min = (d+distance);
				}			
			}				
		}
		e = e->next;			//next edge
	}
	
	if(oldMIN != *min){		//min has changed
		//search all records from queue with the same distance as current record
		r = (*queue)->first->node;
		while(!(empty(*queue)) && getIntegerProperty(0,r->props) == d){					
			bfs(queue,visited1,visited2,node1,node2,g,min);
		}
	}
	
	if((*min) == -1 ){
		(*node1) = current;
	}
	
}
/******************************************************************************/
void create_list(List **l)
{
	(*l) = malloc(sizeof(List));
	(*l)->first = NULL;
	(*l)->last  = NULL;
}
/******************************************************************************/
void destroy_list(List **l)
{	
	while(*l!=NULL && !empty(*l)){
		dequeue(l);
	}
	free(*l);
	(*l) = NULL;
}
/******************************************************************************/
void enqueue(List **q , void *node)
{	
	List_Node *new;
	
	new = malloc(sizeof(List_Node));
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
void* dequeue(List **q)
{
	void *node;	
	List_Node *delete;
	
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
int empty(List *q)
{
	return (q->first == NULL);
}
/******************************************************************************/
void push(List **l,void *node)
{
	List_Node *new;
	
	new = malloc(sizeof(List_Node));
	new->node = node;
	new->next = NULL;
	
	if(empty(*l)){
		(*l)->first = (*l)->last = new;
	}else{
		new->next = (*l)->first ;				
		(*l)->first = new;					
	}
}
/******************************************************************************/
