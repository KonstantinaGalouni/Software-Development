#ifndef GN_H_
#define GN_H_
#include "../graph/graph.h"
#include "../list/list.h"
#include "communities.h"


typedef struct Gn{
	char *forum;
    Communities *results;
}Gn;

Gn **computeGNResults(Graph *graph, double modularity,int N);
int* EdgeBetweenness(Graph *g);
Communities* GirvanNewman(double modularity, Graph* g);
Community* getGnCC(Graph *g, Graph *visited, Record *r);
double calculateModularity(Graph *g);

#endif
