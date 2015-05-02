#include "threadpool.h"
#include "communities.h"
#include <stdlib.h>
#include <stdio.h>


threadpool* poolinit(int tps,List *queue)
{
	threadpool *pool;
	
	if((pool = malloc(sizeof(threadpool))) == NULL){
		perror("Memory Allocation Error\n");
		exit(1);
	}
	
	if((pool->wt = malloc(sizeof(pthread_t) * tps)) == NULL){
		perror("Memory Allocation Error\n");
		exit(1);
	}	

	pool->queue = queue;
	pool->threadpoolexit = 0;
	pool->current = 0;
	
	if(pthread_mutex_init(&(pool->lock1),NULL)) {
		perror("Mutex init error\n");
		exit(1);
	}

	if(pthread_mutex_init(&(pool->lock2),NULL)) {
		perror("Mutex init error\n");
		exit(1);
	}

	if(pthread_cond_init(&(pool->empty),NULL)) {
		perror("Condvar init error\n");
		exit(1);
	}
		
	printf("Threadpool was created! \n");
		
	return pool;
	
}
/*****************************************************************************/
void create_wthreads_Population(threadpool* pool,int tps,int n, Graph *forums)
{	
	int i;
	
	for(i=0;i<tps;i++){	
		ArgsA *arg=malloc(sizeof(ArgsA));		
		arg->N = n;
		arg->forums = forums;
		
		if(pthread_create(&(pool->wt[i]),NULL ,&calculatePopulation,(void*)arg)){
			perror("Thread Creation Error\n");
			exit(1);
		}
	}
	
	printf("Working threads were created! \n");
	
}
/*****************************************************************************/
void create_wthreads_ForumGraph(threadpool* pool,int tps,Graph *g)
{	
	int i;
	
	for(i=0;i<tps;i++){	
		ArgsB *arg=malloc(sizeof(ArgsB));
		arg->index = i;
		arg->g = g;
		if(pthread_create(&(pool->wt[i]),NULL ,&createForumGraph,(void*)arg)){
			perror("Thread Creation Error\n");
			exit(1);
		}
	}
	
	printf("Working threads were created! \n");
	
}
/*****************************************************************************/
void waitworkers(threadpool* pool,int tps)
{
	int i;		
	
	/*wait for workers to join the server's thread*/
	
	for(i=0;i<tps;i++){						
		if(pthread_join(pool->wt[i], NULL) != 0){
			perror("Join thread error\n");
			exit(1);
		}
	}
}	
/*****************************************************************************/	
void destroythreadpool(threadpool* pool)
{		
	destroy_list(&(pool->queue));
	free(pool->wt);
	
	if(pthread_mutex_destroy(&pool->lock1) != 0){
		perror("Mutex destruction error\n");
		exit(1);
	}	
	
	if(pthread_mutex_destroy(&pool->lock2) != 0){
		perror("Mutex destruction error\n");
		exit(1);
	}	
	
	if(pthread_cond_destroy(&(pool->empty)) != 0){
		perror("Cond var destruction error\n");
		exit(1);
	}	
	
	free(pool);
	
}
