#include "GN.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <limits.h>

/******************************************************************************/
Gn **computeGNResults(Graph *graph, double modularity,int N)
{
	int i;
	
	Gn **gn = malloc(N*sizeof(Gn*));
	// find top-N forums and their graphs
	ForumPopulation **topN = computeTopNForums(graph, N);
	Graph** topGraphs = computeTopNGraphs(graph, N);
	
	for(i=0; i<N; i++){
		if(topN[i]->id != -1){
			gn[i] = malloc(sizeof(Gn));
			gn[i]->forum = malloc(sizeof(char)*(strlen(topN[i]->name)+1));
			strcpy(gn[i]->forum, topN[i]->name);
			printf("\nForum %s\n\n",topN[i]->name);
			gn[i]->results = (GirvanNewman(modularity,topGraphs[i]));

			free(topN[i]->name);
			destroyGraph3(&topGraphs[i], 0, 1, -1, -1);
		}else{
			gn[i] = NULL;
		}
		free(topN[i]);
	}
	
	free(topN);
	free(topGraphs);
	return gn;
}
/******************************************************************************/
int* EdgeBetweenness(Graph *g)
{
	Graph *edges = create_Graph(2,3);	
	Bucket *b;
	Record *rec,*r,*w,*u,*s,*start,*end;
	Edge *e;
	Properties *prop;		
	List *queue,*stack,*pred;	
	List_Node *search;
	
	int i,j,d1,d2,s1,s2;
	double deltaw,deltau,max,bc;	
	
	max = -1;		
	
	// Brande's algorithm 
	
	// create graph of edges  --> each record has its neighbors on generic graph
	
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			rec = b->cells[j];			
			rec = create_Node(rec->id,NULL);
			rec->generic = create_Graph(2,3);
			insertNode(rec,edges);
			
		}
	}
	
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			rec = b->cells[j];			
			
			Graph *bet = create_Graph(2,3);							//using graph structure to save necessary info in properties
			create_list(&queue);									//queue for bfs
			create_list(&stack);									//stack to collect results	
			
					
			prop = create_Properties(4);
			setIntegerProperty(0, 0,prop);							//distance from source to source is 0
			setIntegerProperty(1, 1,prop);							//shortest path from source to source is 1
			setDoubleProperty(0, 2,prop);							//dependecy for later accumulation				
			s = create_Node(rec->id,prop);
			insertNode(s,bet);						
			enqueue(&queue,s);					
			
			while(!empty(queue)){									//using bfs to find all shortest paths from s to every other node
				
				u = dequeue(&queue);							
				push(&stack,u);
										
				start = lookupNode(u->id,edges);	
						
				d1 = getIntegerProperty(0,u->props);				//distance from source to u
				
				r = lookupNode(u->id,g);
				e = r->edges;				
				
				while(e != NULL){									//iterate the neighbors of u										
						
						
						if((w = lookupNode(e->id,bet)) == NULL){	//if w is explored for the first time
							prop = create_Properties(4);					
							setIntegerProperty(d1+1, 0,prop);		//distance from source to  w	
							setIntegerProperty(0, 1,prop);			//number of shortest paths from s to w	
							setDoubleProperty(0, 2,prop);			//dependecy for later accumulation											
							create_list(&pred);						//predecessor list
							prop->properties[3] = pred;		
							
							w = create_Node(e->id,prop);																						
							insertNode(w,bet);						//insert to 
							enqueue(&queue,w);				
																								
						}												
						
						// create edge
						
						if((end = lookupNode(e->id,start->generic)) == NULL){						
							prop = create_Properties(1);
							setDoubleProperty(0,0,prop);
							end = create_Node(e->id,prop);							
							insertNode(end,start->generic);						
						}						
						
						d2 = getIntegerProperty(0,w->props);		//distance from source to w
						
						if(d2 == d1 + 1){							 //if edge u->w on shortest path							
							s1 = getIntegerProperty(1,u->props);
							s2 = getIntegerProperty(1,w->props);								
							setIntegerProperty(s1+s2, 1,w->props);	//increase the number of shortest paths							
							pred = w->props->properties[3];
							enqueue(&pred,u);													
						}									
					e = e->next;
				}					
			}		
			while(!empty(stack)){									//collect results
					
				w = dequeue(&stack);							
				s1 = getIntegerProperty(1,w->props);	
				pred = w->props->properties[3];						
				deltaw = getDoubleProperty(2,w->props);				
				
				if(pred != NULL){									//iterate the predecessors													
					search = pred->first;															
					while(search!=NULL){							
						u = search->node;							
						s2 = getIntegerProperty(1,u->props);									
						deltau = getDoubleProperty(2,u->props);								
						deltau = deltau + ((double)s2/s1 )*(1 + deltaw);	
						setDoubleProperty(deltau,2,u->props);	
						
						// increase edge betweenness of u->w
						
						r = lookupNode(w->id,edges);
						r = lookupNode(u->id,r->generic);
						bc = getDoubleProperty(0,r->props);
						bc = bc + ((double)s2/s1 )*(1 + deltaw);
						setDoubleProperty(bc,0,r->props);						
																					
						search = search->next;
					}						
				}					
					
			}
							
			destroy_list(&queue);
			destroy_list(&stack);	
			destroyGraph3(&bet,3,0,3,-1);		
		}
	}
		
	
	int *removed = malloc(sizeof(int)*2);				// edge to be removed
	int x,y;
	Bucket *b1;

	// find edge with max betweenness and return it 	
		
	for(i=0;i<edges->size;i++){
		b = edges->ht[i];
		for(j=0;j<b->nrecs;j++){
			start = b->cells[j];	
			
			for(x=0;x<start->generic->size;x++){
				b1 = start->generic->ht[x];
				for(y=0;y<b1->nrecs;y++){
					end = b1->cells[y];	
					r = lookupNode(end->id,edges);					
					end = lookupNode(end->id,start->generic);
					bc = getDoubleProperty(0,end->props);						
					
					if(bc >= max){						
						max = bc;
						removed[0] = start->id;
						removed[1] = end->id;
					}
			
				}
			}
			
		}
	}
	
	destroyGraph3(&edges, 0, 1, -1, 1);
	
	// proof or right results 
	//if(removed[0] == 15 && removed[1] == 10){
	//	removed[0] = 10;
	//	removed[1] = 13;
	//}
	
	if(max == -1){
		free(removed);
		return NULL;
	}

	return removed;
}
/******************************************************************************/
Communities* GirvanNewman(double modularity, Graph* g)
{
	int i,j,*removed,flag=0;
	Record *r;
	Bucket *b;
	Edge *e;
	double max=-1 ,Q=0;	
	
	do{ 
		removed=EdgeBetweenness(g);
					
		if(removed == NULL){
			printf("No edge found\n");
			exit(0);
		}			
		
		printf("Modularity = %.7lf\t\t",Q);			
		
		removeEdge(removed[0],removed[1],g);
		removeEdge(removed[1],removed[0],g);
		Q = calculateModularity(g);
		
		if(flag != 0 && Q <= max){
			e = setEdgeProperties(removed[0], removed[1], "knows");
			insertEdge(removed[0], e, g);
			
			e = setEdgeProperties(removed[1], removed[0], "knows");
			insertEdge(removed[1], e, g);	
			free(removed);
			break;
		}else{
			max = Q;
			flag = 1;
		}
		
		printf("Removed edge %d -> %d \n",removed[0],removed[1]);	
		free(removed);
	
	}while(Q <= modularity);
	
	printf("\nBroke due to Modularity = %.7lf\n",Q);	
	
	// find all communities (connected components) and their members
	
	int communityid = 0;
	Community *cc;
	Communities *gnResults;
	gnResults = malloc(sizeof(Communities));
	create_list(&gnResults->communities);
	gnResults->numberOfCommunities = 0;
	Graph *visited = create_Graph(2,3);
	
	for(i=0; i<g->size; i++){
		b = g->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];
			if(lookupNode(r->id, visited) == NULL){
				cc = getGnCC(g, visited, r);
				cc->id = communityid;
				enqueue(&gnResults->communities, cc);
				communityid++;
				gnResults->numberOfCommunities++;
			}
			
		}
	}
	
	destroyGraph2(&visited);

	return gnResults;
}
/******************************************************************************/
double calculateModularity(Graph *g)
{
	int i, i1, i2, j, j1, j2;
	Bucket *b, *b1, *b2;
	Record *r, *r1, *r2;
	Edge *e;
	
	if(g->edges == 0){
		return 0;
	}
	
	double mod,sum=0;
	Graph *visited = create_Graph(2,3);
	Graph *cc;
	
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
			
			// create connected compomnent and calculate the sum
			
			if(lookupNode(r->id, visited) == NULL){
				cc = create_Graph(2,3);
				List *queue;	
				create_list(&queue);
				
				insertNode(r, visited);
				enqueue(&queue, r);
				
				while(!empty(queue)){
					r = dequeue(&queue);
					insertNode(r, cc);
			
					e = r->edges;
					while(e!=NULL){
						if(lookupNode(e->id, visited) == NULL){
							r = lookupNode(e->id,g);
							insertNode(r, visited);
							enqueue(&queue, r);
						}
						e = e->next;
					}
				}
				destroy_list(&queue);
				
				for(i1=0;i1<cc->size;i1++){
					b1 = cc->ht[i1];
					for(j1=0;j1<b1->nrecs;j1++){
						r1 = b1->cells[j1];						
						
						for(i2=0;i2<cc->size;i2++){
							b2 = cc->ht[i2];
							for(j2=0;j2<b2->nrecs;j2++){
								r2 = b2->cells[j2];												
								e = r1->edges;
								int a = 0;
								
								// check if nodes are neighbors
								
								while(e!=NULL){									
									if(e->id == r2->id){
										a = 1;
										break;
									}
									e = e->next;
								}
							
								if(g->edges > 0){							
									sum += (a-(((r1->degree/2)*(r2->degree/2))/(double)(g->edges)));							
								}
							}
						}
					}
				}
				
				destroyGraph2(&cc);
			}
		}
	}
	destroyGraph2(&visited);
	
	mod = sum/(double)(g->edges);

	return mod;
}
/******************************************************************************/
Community* getGnCC(Graph *g, Graph *visited, Record *r)
{
	Community *cc = NULL;
	
	List *queue;	
	create_list(&queue);

	Edge *e;	
	
	// using bfs all connected components can be explored		
		
	insertNode(r,visited);
	cc = malloc(sizeof(Community));	
	cc->members = create_Graph(2,3);
	
	enqueue(&queue,r);
	
	r = create_Node(r->id, NULL);
	insertNode(r, cc->members);
								
	while(!empty(queue)){
		r = dequeue(&queue);		
		
		e = r->edges;
		while(e!=NULL){
			if(lookupNode(e->id, visited) == NULL){							
				r = lookupNode(e->id,g);
				insertNode(r,visited);
				enqueue(&queue,r);
				
				r = create_Node(e->id, NULL);
				insertNode(r, cc->members);
			}
			e = e->next;
		}
	}

	destroy_list(&queue);
	
	return cc;
}
/******************************************************************************/
