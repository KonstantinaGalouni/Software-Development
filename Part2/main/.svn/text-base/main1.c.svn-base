#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../graph/graph.h"
#include "main_functions.h"
#include "../metrics/metrics.h"
#include "../queries/queries.h"

int main(int argc , char *argv[])
{
	int i,k;
	Graph *g ;	
	Record* r;	
	
	g = graph_person(2,3);	
	update_person_knows_person(g);	
	//degreeDistribution(g);

	

	r = lookupNode(438,g);
	Matches* match = matchSuggestion(r, 0, 10, 10,7, g);
	for(i=0;i<7;i++){
		printf("match[%d].id = %ld, match[%d].score = %lf\n",i,match[i].id,i, match[i].score);
	}
	free(match);

	k = 8;
	Stalkers *st = malloc(sizeof(Stalkers)*k);
	Graph *stalkers;
	for(i=0;i<k;i++){
		st[i].id = 0;
		st[i].score = -1;
	}
	stalkers = getTopStalkers(k,49,2, g,st);
	for(i=0;i<k;i++){
		printf("stalker id = %ld score = %lf\n",st[i].id,st[i].score);
	}
	printf("density %lf numberofCC %d maxCC %d\n",density(stalkers),numberOfCCs(stalkers),maxCC(stalkers));
	destroyGraph3(&stalkers,0,1,-1,-1);

	free(st);	

	Graph *tr = buildTrustGraph(760, g);
	Record *Reca = lookupNode(956,tr);
	Record *Recb = lookupNode(962,tr);
	printf("estimation = %lf\n",estimateTrust(Reca,Recb, tr));
	
	destroyGraph3(&tr,0,2,-1,-1);

	k=25;
	
	 char** womenTrends = malloc(k*sizeof(char*));	
	 
	 char** menTrends = malloc(k*sizeof(char*));
	 
	 
	 findTrends(k, g, womenTrends, menTrends);

	 printf("\n\nMen Trends\n\n");
	 for(i=0; i<k;i++){
		 printf("%s\n",menTrends[i]);
		 free(menTrends[i]);
	 }
	 printf("\n\nWomen Trends\n\n");
	 for(i=0; i<k;i++){
		 printf("%s\n",womenTrends[i]);
		  free(womenTrends[i]);
	 }

	 free(menTrends);
	 free(womenTrends);

	 destroyGraph(&g);

	return 1;
}
