#ifndef METRICS_H_
#define METRICS_H_
#define INFINITY_REACH_NODE INT_MAX
#include "../graph/graph.h"
#include "../main/main_functions.h"

void degreeDistribution(Graph* g);
int diameter(Graph* g);
int numberOfCCs(Graph* g);
int maxCC(Graph* g);
double averagePathLength(Graph* g);
double density(Graph* g);
double closenessCentrality(Record* r, Graph* g);
double betweennessCentrality(Record* r, Graph* g);

#endif
