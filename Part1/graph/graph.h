#ifndef GRAPH_H_
#define GRAPH_H_
#include "limits.h"
#define INFINITY_REACH_NODE INT_MAX 
#define PERSON_PROPERTIES_NUM 3
#define PERSON_REL_PROPERTIES_NUM 2

typedef struct Properties{
	void **properties;
}Properties;

typedef struct Edge{
        int id;
        Properties *props;
        struct Edge	*next;
}Edge;

typedef struct Record{
        int id;       
        Edge  *edges;
        Properties *props;
}Record;

typedef struct Bucket{
	void **cells;
	int size;
	int nrecs;
}Bucket;

typedef struct Graph{
	void **ht;
	int m;
	int c;
	int p;
	int i;
	int size;	
}Graph;

struct pair {
    int ID; 
    int distance;
};
typedef struct pair Pair;

typedef struct ResultSet{
	Graph *graph;
	int from;	
	int hash;
	int record;
}ResultSet;

Graph*   create_Graph(int m, int c);
int  destroyGraph(Graph **g);

int insertNode(Record *r , Graph *g);
int insertEdge(int id, Edge *e, Graph *g);

Record* lookupNode(int id, Graph *g);

int reachNode1(int start, int end, Graph *g);
int next(ResultSet *r, Pair* p);
ResultSet* reachNodeN(int id, Graph *g);

Bucket* create_Bucket(int c);

int power(int i);
int hash_function(int k, int m, int i);
int sort(Bucket *b , int newid);
void overflow(Bucket *b,int c);
void print_ht(Graph *g);

#endif
