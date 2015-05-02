#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "communities.h"
#include "threadpool.h"
#include "../graph/graph.h"
#include "../list/list.h"

threadpool *pool;
ForumPopulation **topN;
Graph **graphs;

/******************************************************************************/
int sortForumPopulation(ForumPopulation **top,int population,int n)
{	
	int l,r,m;	
	
	r = n - 1;
	l = 0;
	
	m = r/2;
	while(r > l){
		if(top[m]->population < population){
			r = m ;
		}else {
			l = m + 1;
		}
		m = l + ((r-l)/2);
	}
	
	if(top[m]->population > population){
		return n;
	}
	
	if(top[n-1]->name != NULL){
		free(top[n-1]->name);
	}
	free(top[n-1]);
	memmove(&(top[m+1]),&(top[m]),sizeof(ForumPopulation*)*(n-1-m));
	return m;
}
/******************************************************************************/
void *calculatePopulation(void *args)
{	
	int index,*f,forumID1,population;
	ArgsA arg;
	Record *r;
	ForumPopulation *forumP;
	
	memcpy(&arg, args, sizeof(ArgsA));
	
	pthread_mutex_lock(&(pool->lock1));
	pool->current++;
	pthread_mutex_unlock(&(pool->lock1));

	while(1){
		pthread_mutex_lock(&(pool->lock1));		
		
		if(empty(pool->queue)){		
			pthread_mutex_unlock(&(pool->lock1));		
			break;
		}
		
		f = dequeue(&(pool->queue));	// pick one forum from queue
		forumID1 = *f;
		
		pthread_mutex_unlock(&(pool->lock1));	

		r = lookupNode(forumID1, arg.forums);
		population = r->degree;

		pthread_mutex_lock(&(pool->lock2));
		
		index = sortForumPopulation(topN ,population, arg.N);
		if(index < arg.N){				// insert this forum in top-N
			forumP = malloc(sizeof(ForumPopulation));
			forumP->id = forumID1;
			forumP->population = population;
			forumP->name = malloc(strlen(getStringProperty(0, r->props)) +1);
			strcpy(forumP->name, getStringProperty(0, r->props));

			topN[index] = forumP;		
		}	
		free(f);	
		
		pthread_mutex_unlock(&(pool->lock2)); 	
			
	}	

	free(args);	
	
	pthread_mutex_lock(&(pool->lock1));
	pool->current--;
	pthread_mutex_unlock(&(pool->lock1));
	
	if(pool->current == 0){		//no more threads are active
		pthread_cond_broadcast(&(pool->empty));	
	}
	
	pthread_exit(NULL);
	
}
/******************************************************************************/
void *createForumGraph(void *args)
{
	int i,j,forumID,index;
	Graph *g,*visited;
	Bucket *b;
	Record *r,*newRec;
	Edge *e,*newEdge;	
	ArgsB arg;
	
	memcpy(&arg, args, sizeof(ArgsB));
	index = arg.index;
	
	visited = create_Graph(2,3);
	
	forumID = topN[index]->id;

	if(forumID != -1){
		g = create_Graph(2, 3);
		graphs[index] = g;
		
		pthread_mutex_lock(&(pool->lock1));
		pool->current++;
		update_forum_has_member(arg.g);
		pthread_mutex_unlock(&(pool->lock1));
		
		for(i=0; i<arg.g->size; i++){
			b = arg.g->ht[i];
			for(j=0; j<b->nrecs; j++){
				r = b->cells[j];
				
				if(r->forums == NULL || lookupNode(forumID,r->forums) == NULL ){
					continue;
				}
				
				if(lookupNode(r->id, g) == NULL){
					newRec = create_Node(r->id,NULL);
					insertNode(newRec,g);
				}else{
					newRec = r;
				}
				insertNode(newRec, visited);
				
				e = r->edges;
				while(e != NULL){
					if((r=lookupNode(e->id, arg.g)) != NULL){
						if(r->forums != NULL && lookupNode(forumID,r->forums) != NULL && lookupNode(r->id, visited) == NULL){
							if(lookupNode(r->id, g) == NULL){
								r = create_Node(r->id,NULL);
								insertNode(r,g);
							}
							
							newEdge = setEdgeProperties(newRec->id, e->id, getStringProperty(0, e->props));
							insertEdge(newRec->id, newEdge, g);
							
							newEdge = setEdgeProperties(r->id, newRec->id, getStringProperty(0, e->props));
							insertEdge(r->id, newEdge, g);
						}
					}
					e = e->next;
				}
			}
		}
	}		
	free(args);	
	destroyGraph2(&visited);
	
	pthread_mutex_lock(&(pool->lock1));
	pool->current--;
	pthread_mutex_unlock(&(pool->lock1));
	
	if(pool->current == 0){
		pthread_cond_broadcast(&(pool->empty));	
	}

	pthread_exit(NULL);	
}
/******************************************************************************/
ForumPopulation** computeTopNForums(Graph *g, int n)
{
    FILE *fp;
    int i,forumID,personID;
    char buffer[90],*forumName;	
	List *list;
	Graph *forums;
	Record *r;
	Properties* prop;
	
	forums = create_Graph(2,3);

	topN = malloc(sizeof(ForumPopulation*)*n);
	for(i=0; i<n; i++){
		topN[i] = malloc(sizeof(ForumPopulation));
		topN[i]->id = -1;
		topN[i]->population = -1;
		topN[i]->name = NULL;
	}
	
	create_list(&list);	
	pool = poolinit(8,list);

	// find population for each forum with at least one member

    if((fp = fopen("dataset/forum_hasMember_person.csv","r")) == NULL){
        perror("File forum_hasMember_person does not exist!\n");
        exit(1);
    }
	
    fscanf(fp, "%[^\n]",buffer);

    while(fscanf(fp,"%d|%d|%[^\n]",&forumID,&personID,buffer) >= 2){
		if((r = lookupNode(forumID, forums)) == NULL){
			int *forum = malloc(sizeof(int));
			*forum = forumID;		
			enqueue(&(pool->queue) ,forum);
			prop = create_Properties(1);
			r = create_Node(forumID,prop);
			r->degree = 1;			//population
			insertNode(r,forums);
		}else{
			r->degree++;
		}
	}

	fclose(fp);

	//find names for these forums

    if((fp = fopen("dataset/forum.csv","r")) == NULL){
        perror("File forum does not exist!\n");
        exit(1);
    }
	
    fscanf(fp, "%[^\n]",buffer);
    
    while(fscanf(fp,"%d|%[^\n]",&forumID,buffer) == 2){
		if((r = lookupNode(forumID, forums)) != NULL){
			forumName = strtok(buffer, "|");
			setStringProperty(forumName, 0, r->props);
		}
	}

	fclose(fp);

	create_wthreads_Population(pool,8,n,forums);	// find top-N forums

	pthread_mutex_lock(&(pool->lock1));

	while(pool->current > 0){		
		pthread_cond_wait(&(pool->empty),&(pool)->lock1);
	}
		
	pthread_mutex_unlock(&(pool->lock1));
	
	waitworkers(pool,8);
	destroythreadpool(pool);
	destroyGraph3(&forums, 1, -1, -1, -1);

	return topN;	
}
/******************************************************************************/
Graph** computeTopNGraphs(Graph *g, int n){
    int i;
	
	graphs = malloc(n*sizeof(Graph*));

	for(i=0; i<n; i++){
		graphs[i] = NULL;
	}
	
	pool = poolinit(n,NULL);
	create_wthreads_ForumGraph(pool,n,g);	// crate graph for each top-N forum
	
	pthread_mutex_lock(&(pool->lock1));
	
	while(pool->current > 0){		
		pthread_cond_wait(&(pool->empty),&(pool)->lock1);
	}
		
	pthread_mutex_unlock(&(pool->lock1));
	waitworkers(pool,n);
	destroythreadpool(pool);
	
	return graphs;
}
/******************************************************************************/
