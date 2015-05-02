#ifndef COMMUNITIES_H_
#define COMMUNITIES_H_
#include "../graph/graph.h"
#include "../queries/queries.h"
#include "../list/list.h"




typedef enum {False, True} boolean;

typedef struct ForumPopulation{
	int id;
	char *name;
	int population;
}ForumPopulation;

typedef struct Forums{
    ForumPopulation *nForums;
    Graph **gForums;
}Forums;

typedef struct ArgsA{
	Graph *forums;
	int N;
}ArgsA;

typedef struct ArgsB{
	Graph *g;
	int index;
}ArgsB;


typedef struct Community{
	int id;
    Graph *members;
}Community;

typedef struct Communities{
    int numberOfCommunities;
    List *communities;
}Communities;


int sortForumID(ForumPopulation **top,int id,int n);
int sortForumPopulation(ForumPopulation **st,int population,int n);

void *createForumGraph(void *args);
Graph** computeTopNGraphs(Graph *g, int n);
void* calculatePopulation(void *args);
ForumPopulation** computeTopNForums(Graph *g, int n);

#endif
