#ifndef METRICS_H_
#define METRICS_H_
#define INFINITY_REACH_NODE INT_MAX
#include "../graph/graph.h"
#include "../main/main_functions.h"

typedef struct Degree_stats{
	int degree;
	int nrecs;	
}Degree_stats;

int diameter(Graph* g);
int sortDegrees(int *stats,int size,int newdegree);
int numberOfCCs(Graph* g);
int maxCC(Graph* g);

double averagePathLength(Graph* g);
double density(Graph* g);
double closenessCentrality(Record* r, Graph* g);
double betweennessCentrality(Record* r, Graph* g);

void degreeDistribution(Graph* g);

#endif
