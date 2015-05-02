#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h> 
#include "../graph/graph.h"
#include "main_functions.h"
#include "../metrics/metrics.h"
#include "../queries/queries.h"
#include "../algorithms/GN.h"
#include "../algorithms/CPM.h"

#define  NUMBER_OF_TOP_N_FORUMS 1

void free_Gn(Gn **gn);

int main(int argc , char *argv[])
{
	Graph *graph ;
	List_Node *search;
	Community *c;
	Record *r;
	Bucket *b;
	int i,j,x;	
	
	char script[100];
	strcpy(script, "cd dataset; rm -rf *csv; cp main2-example/* .;");
	system(script);
	
	graph = graph_person(2,3);	
	update_person_knows_person(graph);	
	
		
	Gn **gn = computeGNResults(graph, DBL_MAX,NUMBER_OF_TOP_N_FORUMS);	
	
	for(x=0;x<NUMBER_OF_TOP_N_FORUMS;x++){
		if(gn[x] != NULL){
			printf("\nForum Name %s \n",gn[x]->forum);
			search = gn[x]->results->communities->first;
			while(search!=NULL){
				c = search->node;
				printf("\nCommunity ID = %d\tMembers : ",c->id);
				for(i=0;i<c->members->size;i++){
					b = c->members->ht[i];
					for(j=0;j<b->nrecs;j++){
						r = b->cells[j];
						printf("%d ",r->id);
					}
				}
				search = search->next;
				printf("\n");
			}
			
			printf("\n");			
		}
	}
	
	free_Gn(gn);
	destroyGraph(&graph);

	return 1;
}

void free_Gn(Gn **gn){
	int i;
	Communities *c;
	
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; i++){
		if(gn[i] != NULL){
			free(gn[i]->forum);
			c = (gn[i]->results);
			if(c->numberOfCommunities > 0){
				Community *cc;

				List_Node *ln = c->communities->first;
				while(ln != NULL){
					cc = ln->node;
					destroyGraph3(&cc->members,0,0,-1,0);
					free(cc);
					ln = ln->next;
				}
			}
			destroy_list(&c->communities);
			free(c);
			free(gn[i]);
		}
	}
	free(gn);
}
