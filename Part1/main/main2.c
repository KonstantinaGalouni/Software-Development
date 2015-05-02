#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../graph/graph.h"
#include "main_functions.h"

int main(int argc , char *argv[])
{
	int m=2,c=3,i,x;
	Graph *g = create_Graph(m,c);
	
	
	for(i=0;i<15;i++){
		Record* n1 = setPersonProperties(i+1, NULL, NULL, 29); 
		insertNode(n1,g);	
	}	
		
	Edge* e1 = setEdgeProperties(2, 1, "knows", 30);
	Edge* e2 = setEdgeProperties(3, 1, "knows", 20);
	Edge* e3 = setEdgeProperties(8, 1, "knows", 30);
    Edge* e4 = setEdgeProperties(1, 2, "knows", 30);   
    Edge* e5 = setEdgeProperties(4, 2, "knows", 10);
    Edge* e6 = setEdgeProperties(5, 2, "knows", 30);
    Edge* e7 = setEdgeProperties(6, 3, "knows", 30);
    Edge* e8 = setEdgeProperties(7, 3, "knows", 10);
    Edge* e9 = setEdgeProperties(1, 3, "knows", 20);
    Edge* e10 = setEdgeProperties(14, 3, "knows", 30);
    Edge* e11 = setEdgeProperties(2, 4, "knows", 10);
    Edge* e12 = setEdgeProperties(8, 4, "knows", 30);
    Edge* e13 = setEdgeProperties(9, 4, "knows", 50);
    Edge* e14 = setEdgeProperties(10, 4, "knows", 30);
    Edge* e15 = setEdgeProperties(2, 5, "knows", 30);
    Edge* e16 = setEdgeProperties(10, 5, "knows", 30);
    Edge* e17 = setEdgeProperties(11, 5, "knows", 30);
    Edge* e18 = setEdgeProperties(6, 5, "knows", 30);
    Edge* e19 = setEdgeProperties(3, 6, "knows", 30);
    Edge* e20 = setEdgeProperties(5, 6, "knows", 30);
    Edge* e21 = setEdgeProperties(12, 6, "knows", 10);
    Edge* e22 = setEdgeProperties(13, 6, "knows", 30);
    Edge* e23 = setEdgeProperties(9, 6, "knows", 20);
    Edge* e24 = setEdgeProperties(3, 7, "knows", 10);
    Edge* e25 = setEdgeProperties(12, 7, "knows", 30);
    Edge* e26 = setEdgeProperties(14, 7, "knows", 10);
    Edge* e27 = setEdgeProperties(15, 7, "knows", 30);
    Edge* e28 = setEdgeProperties(1, 8, "knows", 30);
    Edge* e29 = setEdgeProperties(4, 8, "knows", 30);
    Edge* e30 = setEdgeProperties(6, 9, "knows", 20);
    Edge* e31 = setEdgeProperties(4, 9, "knows", 50);
    Edge* e32 = setEdgeProperties(14, 10, "knows", 30);
    Edge* e33 = setEdgeProperties(4, 10, "knows", 30);
    Edge* e34 = setEdgeProperties(5, 10, "knows", 30);
    Edge* e35 = setEdgeProperties(5, 11, "knows", 30);
    Edge* e36 = setEdgeProperties(12, 11, "knows", 10);
    Edge* e37 = setEdgeProperties(6, 12, "knows", 10);
    Edge* e38 = setEdgeProperties(7, 12, "knows", 30);
    Edge* e39 = setEdgeProperties(11, 12, "knows", 10);
    Edge* e40 = setEdgeProperties(6, 13, "knows", 30);
    Edge* e41 = setEdgeProperties(3, 14, "knows", 30);
    Edge* e42 = setEdgeProperties(7, 14, "knows", 10);
    Edge* e43 = setEdgeProperties(10, 14, "knows", 30);
    Edge* e44 = setEdgeProperties(7, 15, "knows", 30);    
    
    
    
   
	insertEdge(2, e1, g);
    insertEdge(3, e2, g);
    insertEdge(8, e3, g);
    insertEdge(1, e4, g);
    insertEdge(4, e5, g);
    insertEdge(5, e6, g);
    insertEdge(6, e7, g);
    insertEdge(7, e8, g);
    insertEdge(1, e9, g);
    insertEdge(14, e10, g);
    insertEdge(2, e11, g);
    insertEdge(8, e12, g);
    insertEdge(9, e13, g);
    insertEdge(10, e14, g);
    insertEdge(2, e15, g);
    insertEdge(10, e16, g);
    insertEdge(11, e17, g);
    insertEdge(6, e18, g);
    insertEdge(3, e19, g);
    insertEdge(5, e20, g);
    insertEdge(12, e21, g);
    insertEdge(13, e22, g);
    insertEdge(9, e23, g);
    insertEdge(3, e24, g);
    insertEdge(12, e25, g);
    insertEdge(14, e26, g);    
    insertEdge(15, e27, g);
    insertEdge(1, e28, g);
    insertEdge(4, e29, g);
    insertEdge(6, e30, g);
    insertEdge(4, e31, g);
    insertEdge(14, e32, g);
    insertEdge(4, e33, g);
    insertEdge(5, e34, g);
    insertEdge(5, e35, g);
    insertEdge(12, e36, g);
    insertEdge(6, e37, g);
    insertEdge(7, e38, g);
    insertEdge(11, e39, g);
    insertEdge(6, e40, g);
    insertEdge(3, e41, g);
    insertEdge(7, e42, g);
    insertEdge(10, e43, g);
    insertEdge(7, e44, g);
      
		
	
	printf("Give id for reachNodeN!\n");
	scanf("%d",&x);
	
	ResultSet* res = reachNodeN(x, g);
	Pair p;
	
	while(next(res,&p)){
		printf("Distance between %d and %d = %d\n",x,p.ID,p.distance);
	}	
	free(res);
	destroyGraph(&g);	
}
