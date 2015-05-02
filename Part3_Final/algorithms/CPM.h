#ifndef CPM_H_
#define CPM_H_
#include "../graph/graph.h"
#include "../list/list.h"
#include "communities.h"


typedef struct Cpm{
	char *forum;
    Communities *clique3;
    Communities *clique4;
}Cpm;

typedef struct Treenode{
	int id;	
	Graph *visited;
	List *children;
	struct Treenode *parent;
}Treenode;
	

Cpm **computeCPMResults(Graph *graph, int *cliqueSize,int N);
int explore(Treenode* treenode,Graph *clique,Graph *neighbors, Graph *visited,List *results,int level,int k);
Communities* cliquePercolationMethod(int k, Graph *g);
Community* getCC(Graph *allcliques, Graph *visited, Record *hypernode, int communityid);
void destroyAllTrees(Treenode ***trees, int counter);
void destroyTree(Treenode **tree);
#endif
