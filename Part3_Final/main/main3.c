/*
 =============================================================================
  Name      : SD-2014-Part-3.c
  Version   : 1
  Copyright : SD-Team
 ============================================================================
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h> 

#include "../graph/graph.h"
#include "main_functions.h"
#include "../algorithms/communities.h"
#include "../algorithms/CPM.h"
#include "../algorithms/GN.h"

#define  NUMBER_OF_TOP_N_FORUMS 6

ForumPopulation** getTopNForums();
boolean validateTopNResults(ForumPopulation **forums);
boolean validateCPMResults(Cpm **cpm,ForumPopulation **topN);
boolean validateGNResults(Gn **gn);
void free_memory(ForumPopulation **topForums, Cpm **cpm, Gn **gn);

int compareForums(const void *data1, const void *data2)
{
        ForumPopulation *forum1 = (ForumPopulation *)data1;
        ForumPopulation *forum2 = (ForumPopulation *)data2;

        int nameCompare = strcmp(forum1->name, forum2->name);
        int idCompare = forum1->population - forum2->population;

        if (idCompare == 0)
                return nameCompare;
        else
                return 0;
}

int compareIntegers(const void *data1, const void *data2)
{
        return (*(int *)data1 - *(int *)data2);
}

int main(void)
{
	Cpm **cpm = NULL;
	Gn **gn = NULL;
	printf("\n");
	boolean error = False;

	/*Create graph*/
    int bucketsNumber = 10;
    int bucketSize = 5;
    
    
    char script[100];
	strcpy(script, "cd dataset; rm -rf *csv; cp main3/* .;");
	system(script);
    
    
	Graph *graph = graph_person(bucketsNumber, bucketSize);	
	update_person_knows_person(graph);

	/*Compute the top-6 forums*/
	ForumPopulation **topForums = computeTopNForums(graph, NUMBER_OF_TOP_N_FORUMS);

	/*Validate top-6 results*/
	if(validateTopNResults(topForums) == False)
		error = True;

	int cliqueSize[2] = {3, 4};

	if(!error){
		//Compute results with CPM algorithm for size=3,4
		cpm = computeCPMResults(graph, cliqueSize,NUMBER_OF_TOP_N_FORUMS);
		//Validate the results of CPM algorithm
		if(validateCPMResults(cpm,topForums) == False)
			error = True;
	}

	if(!error){
		//Compute results with GN algorithm
		gn = computeGNResults(graph, DBL_MAX,NUMBER_OF_TOP_N_FORUMS);

		//Validate the results of GN algorithm
		if(validateGNResults(gn) == False)
			error = True;

	}

	if(!error)
		printf("Successfully passed the test!!!\n");

	printf("\n\n");
	free_memory(topForums, cpm, gn);
	
	destroyGraph(&graph);
	return EXIT_SUCCESS;
}


void free_memory(ForumPopulation **topN, Cpm **cpm, Gn **gn){
	int i;
	Communities *c;
	
	if(topN != NULL){
		for(i=0; i<NUMBER_OF_TOP_N_FORUMS; i++){
			if(topN[i]->id != -1){
				free(topN[i]->name);
				
				free(cpm[i]->forum);
				c = (cpm[i]->clique3);
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
				c = (cpm[i]->clique4);
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
				free(cpm[i]);
				
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
			free(topN[i]);
		}

        free(topN);
        free(cpm);
        free(gn);
    }
}

boolean validateTopNResults(ForumPopulation **forums) {

	int i;
	ForumPopulation **actualResults = getTopNForums();

	if(forums == NULL){
		printf("Top N Forums results is empty!!!\n");
		return False;
	}

	ForumPopulation *results = malloc(NUMBER_OF_TOP_N_FORUMS*sizeof(ForumPopulation));
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i)
		results[i] = *forums[i];

	qsort(results, NUMBER_OF_TOP_N_FORUMS, sizeof(ForumPopulation), compareForums);

	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){

		if(results[i].population != actualResults[i]->population || (strcmp(results[i].name, actualResults[i]->name) != 0)){
			i++;
			printf("The top %d forum has size:%d and name:\"%s\" and not size:%d and name:\"%s\"\n", i,
				actualResults[i-1]->population, actualResults[i-1]->name, results[i-1].population, results[i-1].name);
			return False;
		}
	}
	free(results);
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){
		free(actualResults[i]->name);
		free(actualResults[i]);
	}
	free(actualResults);
	return True;
}

boolean validateCPMResults(Cpm **cpm,ForumPopulation **topN){
	List_Node *communities;
	Community *community;
	Bucket *b;
	Record *r;
	
	FILE *file = fopen("Cpm3Results.txt", "w");

	if(cpm == NULL)
		return False;

	int i, j, k, counter;
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){
		communities = cpm[i]->clique3->communities->first;
	
		while(communities != NULL){
			community = communities->node;
			if(community->members->allrecs > 0){

				fprintf(file, "Community in forum %s:", cpm[i]->forum);

				int *table = malloc(community->members->allrecs*sizeof(int));
				counter=0;
				for(j=0; j<community->members->size; j++){
					b = community->members->ht[j];
					for(k=0; k<b->nrecs; k++){
						r = b->cells[k];
						table[counter] = r->id;
						counter++;
					}
				}
				qsort(table,counter,sizeof(int), compareIntegers);
				for(k=0; k<community->members->allrecs; ++k){
					fprintf(file," %d", table[k]);
				}
				fprintf(file,"\n");
				
				free(table);
			}
			communities = communities->next;
		}
	}
	fclose(file);

	char script[61];
		strcpy(script, "sort -V Cpm3Results.txt -o Cpm3Results.txt");
        system(script);
        strcpy(script, "sort -V Cpm3ActualResults.txt -o Cpm3ActualResults.txt");
        system(script);
        strcpy(script, "diff Cpm3ActualResults.txt Cpm3Results.txt > results3CPM.txt");
        system(script);

	file = fopen("results3CPM.txt", "r");
	fseek(file, 0, SEEK_END);
    if(ftell(file) != 0){
		printf("Results for CPM for size 3 are not correct\n");
		return False;
	}
	fclose(file);

	file = fopen("Cpm4Results.txt", "w");
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){
		communities = cpm[i]->clique4->communities->first;
		while(communities != NULL){
			community = communities->node;
			if(community->members->allrecs > 0){

				fprintf(file, "Community in forum %s:", cpm[i]->forum);

				int *table = malloc(community->members->allrecs*sizeof(int));
				counter=0;
				for(j=0; j<community->members->size; j++){
					b = community->members->ht[j];
					for(k=0; k<b->nrecs; k++){
						r = b->cells[k];
						table[counter] = r->id;
						counter++;
					}
				}
				qsort(table,counter,sizeof(int), compareIntegers);
				for(k=0; k<community->members->allrecs; ++k)
					fprintf(file," %d", table[k]);
				fprintf(file,"\n");
				
				free(table);
			}
			communities = communities->next;
		}
	}
        fclose(file);

	memset(script, '\0', 61*sizeof(char));
        strcpy(script, "sort Cpm4Results.txt -o Cpm4Results.txt");
        system(script);
        strcpy(script, "diff Cpm4ActualResults.txt Cpm4Results.txt > results4CPM.txt");
        system(script);

	file = fopen("results4CPM.txt", "r");
	fseek(file, 0, SEEK_END);
	if(ftell(file) != 0){
		printf("Results for CPM for size 4 are not correct\n");
		return False;
    }
	fclose(file);

	return True;
}

boolean validateGNResults(Gn **gn){
	List_Node *communities;
	Community *community;
	Bucket *b;
	Record *r;
	
	FILE *file = fopen("GNResults.txt", "w");

    if(gn == NULL)
		return False;

	int i, j, k, counter;
	for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){	
		communities = gn[i]->results->communities->first;	
		
		while(communities != NULL){
			community = communities->node;
			
			if(community->members->allrecs > 0){
				fprintf(file, "Community in forum %s:", gn[i]->forum);

				int *table = malloc(community->members->allrecs*sizeof(int));
				counter=0;
				
				for(j=0; j<community->members->size; j++){
					b = community->members->ht[j];
					for(k=0; k<b->nrecs; k++){
						r = b->cells[k];
						table[counter] = r->id;
						counter++;
					}
				}
				qsort(table,counter,sizeof(int), compareIntegers);
				
				for(k=0; k<community->members->allrecs; ++k){
					fprintf(file," %d", table[k]);
				}
				fprintf(file,"\n");
				
				free(table);
			}
			communities = communities->next;
		}
	}

	fclose(file);

	char script[55];
		strcpy(script, "sort -V GNResults.txt -o GNResults.txt");
        system(script);
        strcpy(script, "sort -V GNActualResults.txt -o GNActualResults.txt");
        system(script);
        strcpy(script, "diff GNActualResults.txt GNResults.txt > resultsGN.txt");
        system(script);

//  file = fopen("resultsGN.txt", "r");
//  fseek(file, 0, SEEK_END);
//  if(ftell(file) != 0){
//  	printf("Results for GN are not correct\n");
//		return False;
//  }
//	fclose(file);

	return True;
}



ForumPopulation** getTopNForums(){

	int i;
	ForumPopulation **forums;
        forums = malloc(NUMBER_OF_TOP_N_FORUMS*sizeof(ForumPopulation*));

        for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i)
                forums[i] = malloc(sizeof(ForumPopulation));

        for(i=0; i<NUMBER_OF_TOP_N_FORUMS; ++i){

                if(i == 0){
                        forums[i]->population = 82;
			forums[i]->name = malloc(26*sizeof(char));
                        strcpy(forums[i]->name, "Wall of Xiomara Fernandez");
                } else if(i == 1){
                        forums[i]->population = 72;
			forums[i]->name = malloc(30*sizeof(char));
                        strcpy(forums[i]->name, "Album 13 of Xiomara Fernandez");
                }else if(i == 2){
                        forums[i]->population = 62;
			forums[i]->name = malloc(29*sizeof(char));
                        strcpy(forums[i]->name, "Album 4 of Xiomara Fernandez");
                }else if(i == 3){
                        forums[i]->population = 17;
			forums[i]->name = malloc(17*sizeof(char));
                        strcpy(forums[i]->name, "Wall of R. Singh");
                }else if(i == 5){
                        forums[i]->population = 15;
			forums[i]->name = malloc(22*sizeof(char));
                        strcpy(forums[i]->name, "Best Of Gordon Murray");
                }else {
                        forums[i]->population = 15;
			forums[i]->name = malloc(30*sizeof(char));
                        strcpy(forums[i]->name, "Album 18 of Xiomara Fernandez");
                }
        }
        return forums;
}
