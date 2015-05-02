#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../graph/graph.h"
#include "main_functions.h"


int main(int argc , char *argv[])
{
	int m=2,c=3,i,x,y;
	Graph *g = create_Graph(m,c);
	Record *prev = NULL;
	
	for(i=0;i<1000000;i++){
		Record* n1 = setPersonProperties(i+1, NULL, NULL, 29); 
		insertNode(n1,g);	
		if(prev != NULL){
			Edge *e1 = setEdgeProperties(0, n1->id, "knows", 30);
			Edge *e2 = setEdgeProperties(0, prev->id, "knows", 30);
			insertEdge(n1->id,e2,g);
			insertEdge(prev->id,e1,g);
		}
		prev = n1;
	}
	printf("Give starting and ending point!\n");
	scanf("%d",&x);
	scanf("%d",&y);
	printf("reachnode1 = %d\n", reachNode1(x, y, g));
	destroyGraph(&g);
}
