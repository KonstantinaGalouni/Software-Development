#include <pthread.h>
#include <unistd.h>
#include "../list/list.h"
#include "communities.h"

typedef struct threadpool {
	pthread_cond_t empty;
	pthread_mutex_t lock1;	
	pthread_mutex_t lock2;	
	pthread_t *wt;				/*working threads*/
    List *queue; 				/*job queue*/
    int threadpoolexit;
    int current;
}threadpool;


threadpool* poolinit(int tps,List *queue);
void create_wthreads_Population(threadpool* pool,int tps,int n,Graph *forums);
void create_wthreads_ForumGraph(threadpool* pool,int tps,Graph *g);
void destroythreadpool(threadpool* pool);
void waitworkers(threadpool* pool,int tps);
