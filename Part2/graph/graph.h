#ifndef GRAPH_H_
#define GRAPH_H_
#include "limits.h"
#define INFINITY_REACH_NODE INT_MAX 
#define PERSON_PROPERTIES_NUM 7
#define PERSON_REL_PROPERTIES_NUM 1

typedef struct Properties{
	void **properties;
}Properties;

typedef struct Edge{
        int id;
        Properties *props;
        struct Edge	*next;
}Edge;

typedef struct Bucket{
	void **cells;
	int size;
	int nrecs;
}Bucket;

typedef struct Graph{
	void **ht;	
	int size;
	int edges;
	int allrecs;
	int m;								/*linear hashing variables*/
	int c;
	int p;
	int i;	
}Graph;

typedef struct Record{
	int id;
	int degree;
	Edge  *edges;		
	Properties *props;
	Graph *likes;					// likes from record to other people (holds forums on generic graph)
	Graph *forums;					// forums record belongs to
	Graph *interests;				// interests a record has
	Graph *generic;					// multipurpose generic graph
}Record;

typedef struct Pair {
    int ID; 
    int distance;
}Pair;

typedef struct ResultSet{
	int from;	
	int hash;
	int record;
	Graph *graph;
}ResultSet;

Graph*   create_Graph(int m, int c);
int  destroyGraph(Graph **g);

int insertNode(Record *r , Graph *g);
int insertEdge(int id, Edge *e, Graph *g);

Record* lookupNode(int id, Graph *g);

int reachNode1(int start, int end, Graph *g);
int next(ResultSet *r, Pair *p,Graph *visited);
ResultSet* reachNodeN(int id, Graph *g);

Bucket* create_Bucket(int c);
int power(int i);
int hash_function(int k, int m, int i);
int sort(Bucket *b , int newid);
void overflow(Bucket *b,int c);
void print_ht(Graph *g);
void destroyGraph2(Graph **g);
void destroyGraph3(Graph **g,int maxprop,int maxedgeprop,int listindex,int maxpropInternal);

#endif
