#ifndef QUERIES_H_
#define QUERIES_H_
#include "../metrics/metrics.h"

typedef struct Matches{
	long id;
	double score;
}Matches,Stalkers;

Matches* matchSuggestion(Record *n, int k, int h, int x, int limit, Graph *g);

Graph* getTopStalkers(int k, int x, int centralityMode, Graph *g, Stalkers *st);
Graph* buildTrustGraph(int forumID, Graph *g);

int find_maxCC(Graph *g,Graph *gender,Record *tag,int index);
int sortTrends(int *st,int score,int k);
int sortMatches(Stalkers *st,double score,int k);

double estimateTrust(Record *a, Record *b, Graph *trustGraph);
void findTrends(int k, Graph *g, char **womenTrends, char **menTrends);

#endif
