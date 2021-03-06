#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "../queue/queue.h"

/******************************************************************************/
Graph* create_Graph(int m, int c)
{	
	int i;
	Graph *g;
			
	g = malloc(sizeof(struct Graph));
	//initialize variables for linear hashing
	g->m = m;
	g->c = c;
	g->p = 0; 
	g->i = 0;
	g->size = m;						//at start hash table contains m buckets
	g->ht = (void*)malloc(sizeof(void*)*m);
	
	for(i=0;i<m;i++){	
		g->ht[i] = create_Bucket(c);	//at start each bucket contains c cells
	}
	
	return g;
}
/******************************************************************************/
int destroyGraph(Graph **g)
{
	int i,j,k;
	Bucket *b;
	Record *r;
	Edge *e;
	
	for(i=0; i<(*g)->size; i++){
		//for all buckets of hash table
		b = (*g)->ht[i];
		for(j=0; j<b->size; j++){
			//for all records of bucket b
			r = b->cells[j];
			if(r!=NULL){
				while(r->edges != NULL){
					//for all edges of record r
					e = r->edges;
					r->edges = r->edges->next;
									
					for(k=0;k<PERSON_REL_PROPERTIES_NUM;k++){
						//for all properties of edge e
						free(e->props->properties[k]);						
					}
					free(e->props->properties);
					free(e->props);
					free(e);
				}
				
				for(k=0;k<PERSON_PROPERTIES_NUM;k++){
					//for all properties of record r
					if(r->props->properties[k]!=NULL){
						free(r->props->properties[k]);
					}
				}
				free(r->props->properties);
				free(r->props);
			}else{
				break;
			}
			free(r);
		}
		free(b->cells);
		free(b);
	}
	free((*g)->ht);
	free(*g);
	(*g) = NULL;
	
	return 1;	//success
}
/******************************************************************************/
int insertNode(Record *r , Graph *g)
{
	Bucket  *b;
	int hash,i;
	
	hash = hash_function(r->id,g->m,g->i);	//choose bucket to put the record

	if(hash < g->p){
		hash = hash_function(r->id, g->m, g->i+1);
	}

	b = g->ht[hash];
	if(b->nrecs == b->size){				//bucket if full
		int index1=0,index2=0;
		Record *temp;
		Bucket *new = create_Bucket(g->c);
		Bucket *split = g->ht[g->p];		//split bucket is the one pointed by p
		
		g->size++;							//hash table gains one more bucket
		g->ht = realloc(g->ht,g->size*sizeof(void*));
		g->ht[g->size-1] = new;
				
		
		for(i=0; i<split->size; i++){		//split all records of split bucket
			if(split->cells[i]==NULL){
				break;
			}
			
			temp = split->cells[i];
			
			if(hash_function(temp->id, g->m, g->i+1) == g->p){
				//record remains in the same bucket
				split->cells[i] = NULL;
    			split->cells[index1] = temp;  							
				index1++;
			}else{
				//record changes bucket and goes to the new one
				if(index2 == new->size){
					//new bucket is full -> overflow page
					overflow(new,g->c);
				}				
				new->cells[index2] = temp;
				new->nrecs++;		//new bucket contains one more record
				split->cells[i] = NULL;
				split->nrecs--;		//split bucket contains one less record
				index2++;
			}
		}
		
		if(hash == g->p){
			hash = hash_function(r->id,g->m,g->i+1);
			b = g->ht[hash];
		}
		
		if(b->nrecs == b->size){	//new record's bucket is still full
			overflow(b,g->c);		//overflow page
		}	

		g->p++;						//p points at the next bucket of hash table
		if(g->size == power(g->i+1)*g->m){	//hash table is doubled
			g->i++;					//new round
			g->p = 0;				//p points at the first bucket
		}		
	}
	
	b->cells[sort(b,r->id)] = r;	//insertion sort
	b->nrecs++;						//b contains one more record

	return 1;
}
/******************************************************************************/
int insertEdge(int id, Edge *e, Graph *g)
{	
	Record *r;	
	
	if(e == NULL || (r = lookupNode(id, g)) == NULL){
		// edge is NULL or record doesn't exist in hash table -> fail
		return 0;
	}

	if(r->edges != NULL){
		e->next = r->edges;		//edge is placed first
	}
	
	r->edges = e;
	
	return 1;
}
/******************************************************************************/
Record* lookupNode(int id, Graph *g)
{
	int hash,low, high ,mid;
	Record *search;
	Bucket *b;	

	hash = hash_function(id, g->m, g->i);	//choose bucket to search the record
	
	if(hash < g->p){
		hash = hash_function(id, g->m, g->i+1);
	}
	
	b = g->ht[hash];
	
	low  = 0; 
	high = b->nrecs-1;
	
	//binary search
	while(low <= high){
		mid = (low + high)/2;
		search = b->cells[mid]; 
		
		if(search->id < id){
			low = mid + 1; 
		}else if(search->id == id){		
			return search;
		}else{
			high = mid-1;
		}		
	}
	
	return NULL;
}
/******************************************************************************/
int reachNode1(int start, int end, Graph *g)
{	
	Queue *queue1,*queue2;
	
	int m=2,c=3,d1,d2,min=-1;
	Graph *visited1 = create_Graph(m,c);
	Graph *visited2 = create_Graph(m,c);

	Record *node1,*node2;
	
	create_queue(&queue1);
	create_queue(&queue2);	
	
	node1 = setPersonProperties(start, NULL, NULL, 0);
	insertNode(node1, visited1);
	node2 = setPersonProperties(end, NULL, NULL, 0);
	insertNode(node2, visited2);
	
	enqueue(&queue1,node1);
	enqueue(&queue2,node2);
	
	//bidirectional breadth first search
	while(node1->id!=node2->id){		
		bfs(&queue1,visited1, visited2,&node1,&node2,g,&min);	//first path
		if(node1 == NULL || node1->id == end){
			//path 1 is a deadlock (no more nodes to explore) or reached the end
			break;
		}			
			
		if(node1->id == node2->id){
			//the two paths explored the same node, solution found
			break;
		}
		
		bfs(&queue2,visited2, visited1,&node2,&node1,g,&min);				
		if(node2 == NULL || node2->id == start){
			//path 2 is a deadlock (no more nodes to explore) or reached the start
			break;
		}
	}	
	
	if(node1 != NULL && node1->id == end){
		//minimum distance is the distance of node1 from the start
		d1 = getIntegerProperty(2, node1->props); 
		d2 = 0;
	}
	if(node2 != NULL && node2->id == start){
		//minimum distance is the distance of node2 from the end
		d1 = 0;
		d2 = getIntegerProperty(2, node2->props);
	}
	if(node1 != NULL && node2 != NULL && node1->id == node2->id){
		//minimum distance is the distance of node1 plus distance of node2
		d1 = getIntegerProperty(2, node1->props);
		d2 = getIntegerProperty(2, node2->props);
	}
	if(node1 == NULL || node2 == NULL){
		//there is no path between these nodes
		d1 = INFINITY_REACH_NODE;
		d2 = 0;
	}
	
	destroyGraph(&visited1);
	destroyGraph(&visited2);
	destroy_queue(&queue1);
	destroy_queue(&queue2);	
	
	return d1+d2;	
}
/******************************************************************************/
int next(ResultSet *r, Pair *p)
{
	Bucket *b;
	Record *rec;

	b = r->graph->ht[r->hash];
	r->record++;

	while(r->record == b->nrecs){
		//if all records of this bucket were exlpored, change bucket
		r->record = 0;
		r->hash++;
		if(r->hash == r->graph->size){		//if there are no other buckets
			return 0;						//no more records are available
		}
		b = r->graph->ht[r->hash];
	}
	
	rec = b->cells[r->record];				//explore record rec
		
	p->ID = rec->id;
	p->distance = reachNode1(r->from, rec->id, r->graph);

	if(p->distance == INFINITY_REACH_NODE || p->distance == 0){
		//don't put the starting node or not reachable nodes at the pair
		return next(r, p);
	}
	
	return 1;
}
/******************************************************************************/
ResultSet* reachNodeN(int id, Graph *g)
{
	ResultSet *r = malloc(sizeof(ResultSet));

	r->from = id;
	r->graph = g;
	r->hash = 0;
	r->record = -1;
	
	return r;
}
/******************************************************************************/
Bucket* create_Bucket(int c)
{
	int j;
	Bucket* new;

	new = malloc(sizeof(struct Bucket));
	new->cells = malloc(sizeof(Record*)*c);
	new->size = c;			//at start each bucket contains c cells
	new->nrecs = 0;			//at start each bucket contains 0 records
	
	for(j=0;j<c;j++){
		new->cells[j] = NULL;
	}
	
	return new;
}
/******************************************************************************/
int power(int i)
{
	int j,p=1;							
	for(j=0;j<i;j++){					
		p *= 2;							
	}
	return p;
}
/******************************************************************************/
int hash_function(int k, int m, int i)
{	
	return k%(power(i)*m);
}
/******************************************************************************/
int sort(Bucket *b , int newid)
{
	int r,l,m;
	Record *rec;
	
	r = b->nrecs;
	l = 0;
	
	if(r == 0){				//bucket is empty
		return 0;			//new record is placed at the first cell
	}
	
	rec = b->cells[b->nrecs-1];
	if(newid > rec->id){	//record's id is greater than bucket's last record
		return b->nrecs;	//new record is placed at the last cell
	}
	rec = b->cells[0];		
	
	if(newid < rec->id){	//record's id is lower than bucket's first record
		memmove(&(b->cells[1]),&(b->cells[0]),sizeof(Record*)*b->nrecs);
		return 0;			//new record is placed at the first cell
	}
	
	//binary sort
	while(r-l > 1){
		m = l + (r-l)/2;
		rec = b->cells[m];		
		if(rec->id > newid){
			r = m;
		}else{
			l = m;
		}
	}
	
	memmove(&(b->cells[m+1]),&(b->cells[m]),sizeof(Record*)*(b->nrecs-m));
	
	return m;
}
/******************************************************************************/
void overflow(Bucket *b,int c)
{	
	int i;
	
	b->size += c;				//bucket b gets c more cells
	b->cells = realloc(b->cells,b->size*sizeof(Record*));					
	for(i=b->size-c;i<b->size;i++){
		b->cells[i] = NULL;
	}
}
/******************************************************************************/
void print_ht(Graph *g)
{
	int i=0,j;
	
	while(i < g->size){		
		Bucket *b;
		b = g->ht[i];
		Record *rr;		
		j=0;
		
		printf(" _________________");
		printf("\n|   Bucket %5d  |\n",i);
		printf("|   Size %7d  |\n",b->size);	
		printf("|_________________|\n");
		
		while(j<b->nrecs){
			rr = b->cells[j];
			if(rr != NULL){
				printf("|    %5d\t  |\n",rr->id);
				printf("|_________________|\n");
			}else{
				break;
				
			}
			j++;
		}		
		printf("\n\n");		
		i++;
	}
}
/******************************************************************************/
