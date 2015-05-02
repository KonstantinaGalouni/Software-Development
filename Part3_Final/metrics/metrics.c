#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../metrics/metrics.h"
#include "../list/list.h"

/******************************************************************************/
void degreeDistribution(Graph* g)
{	
	FILE *fp;	
    FILE *gnuplotPipe = popen ("gnuplot -persistent", "w");
    char *commandsForGnuplot[] = {"set title \"Distribution\"", "plot 'Degree Distribution' with lines"};
    
    int i,j;
	double temp;
	Bucket *b;
	Record *r,*n;
	Graph *degrees = create_Graph(2,3);    			
    
    if((fp = fopen("Degree Distribution", "w")) == NULL){
		perror("File Degree Distribution could not open!\n");
		exit(1);
	}

	// find all different degrees and their number of appearences in records

	for(i=0; i<g->size; i++){					 		
		b = g->ht[i];		
		for(j=0; j<b->nrecs; j++){						
			r = b->cells[j];
			if(r!=NULL){
				if((n = lookupNode(r->degree, degrees)) == NULL){
					n = setPersonProperties(r->degree, NULL, NULL, NULL, NULL, NULL, NULL, NULL);															
					insertNode(n, degrees);
				}
				n->degree++;				
			}
		}
	}	
	
	for(i=0;i<degrees->size;i++){
		b = degrees->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
			temp = r->degree/(double)g->allrecs;
			fprintf(fp, "%d %lf\n", r->id, temp);
		}
	}
	fclose(fp);
	
	// sort them in descending order
	char script[60];
	strcpy(script, "sort -V 'Degree Distribution' -o 'Degree Distribution'");
	system(script);
	
	// send commands to gnuplot one by one.
	for (i=0; i < 2; i++){
		fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); 
    }
    
    pclose(gnuplotPipe);
	destroyGraph(&degrees);	
}
/******************************************************************************/
int diameter(Graph* g)
{
	Bucket *b;
	Record *r,*node;
	Edge *e;
	Properties *props;
	
	int i,j,diameter=0,distance;
	
	//use bfs to find max of shortest paths
		
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
					
			Graph *visited;
			visited = create_Graph(2,3);
			List *queue;	
			create_list(&queue);
			
			props = create_Properties(1);
			setIntegerProperty(0,0,props);
			node = create_Node(r->id,props);			
			enqueue(&queue,node);
			insertNode(node,visited);
					
			while(!empty(queue)){
				node = dequeue(&queue);

				r = lookupNode(node->id, visited);
				distance = getIntegerProperty(0,r->props);
				
				r = lookupNode(node->id,g);
				e = r->edges;
				
				while(e!=NULL){
					if(lookupNode(e->id, visited) == NULL){						
						props = create_Properties(1);
						setIntegerProperty(distance+1,0,props);
						node = create_Node(e->id,props);
						insertNode(node,visited);
						enqueue(&queue,node);
						
						if(distance + 1 > diameter){
							diameter = distance + 1;
						}						
					}
					e = e->next;
				}
			}
				
			destroyGraph3(&visited, 1, 0, -1, -1);
			destroy_list(&queue);
		}
			
	}	
	return diameter;	
}
/******************************************************************************/
double averagePathLength(Graph* g)
{
	int i,j,distance,allpaths = 0,sum = 0;		
	Bucket *b;
	Record *r,*node;
	Edge *e;
	Properties *props;			
	
	// use bfs to sum distance for all shortest paths
		
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
					
			Graph *visited;
			visited = create_Graph(2,3);
			List *queue;	
			create_list(&queue);
			
			props = create_Properties(1);
			setIntegerProperty(0,0,props);
			node = create_Node(r->id,props);			
			enqueue(&queue,node);				
			insertNode(node,visited);
					
			while(!empty(queue)){
				node = dequeue(&queue);

				r = lookupNode(node->id, visited);
				distance = getIntegerProperty(0,r->props);

				r = lookupNode(node->id,g);
				e = r->edges;
				
				while(e!=NULL){
					if(lookupNode(e->id, visited) == NULL){						
						props = create_Properties(1);
						setIntegerProperty(distance+1,0,props);
						node = create_Node(e->id,props);
						insertNode(node,visited);
						enqueue(&queue,node);
						
						allpaths++;
						sum += distance+1;												
					}
					e = e->next;
				}
			}
			
			destroyGraph3(&visited, 1, 0, -1, -1);
			destroy_list(&queue);
		}
			
	}
	
	if(allpaths != 0){
		return sum/(double)allpaths;
	}else{
		return 0;
	}	
}
/******************************************************************************/
int maxCC(Graph* g)
{
	Graph *visited;
	visited = create_Graph(2,3);
	
	List *queue;	
	create_list(&queue);
	
	Bucket *b;
	Record *r,*node;
	Edge *e;
	
	int i,j,n,max=0;	
	
	// using bfs all connected components and their members can be explored
		
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];			
			if(lookupNode(r->id,visited) == NULL){				
				insertNode(r,visited);
				enqueue(&queue,r);				
				n = 1;				
				while(!empty(queue)){
					node = dequeue(&queue);
					r = lookupNode(node->id,g);
					e = r->edges;
					while(e!=NULL){
						if(lookupNode(e->id, visited) == NULL){							
							r = lookupNode(e->id,g);
							insertNode(r,visited);
							enqueue(&queue,r);
							n++;
						}
						e = e->next;
					}
				}
				if(n > max ){
					max = n;
				}			
			}
		}
			
	}
	
	destroyGraph2(&visited);
	destroy_list(&queue);
	
	return max;
}
/******************************************************************************/
int numberOfCCs(Graph* g)
{
	Graph *visited;
	visited = create_Graph(2,3);
	
	List *queue;	
	create_list(&queue);
	
	Bucket *b;
	Record *r,*node;
	Edge *e;
	
	int cc=0,i,j;	
	
	// using bfs all connected components can be explored
		
	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
			if(lookupNode(r->id,visited) == NULL){		
				insertNode(r,visited);
				enqueue(&queue,r);
				cc++;								
				while(!empty(queue)){
					node = dequeue(&queue);
					r = lookupNode(node->id,g);
					e = r->edges;
					while(e!=NULL){
						if(lookupNode(e->id, visited) == NULL){						
							r = lookupNode(e->id,g);
							insertNode(r,visited);
							enqueue(&queue,r);
						}
						e = e->next;
					}
				}					
			}
		}
	}
	
	destroyGraph2(&visited);
	destroy_list(&queue);
	
	return cc;
}
/******************************************************************************/
double density(Graph* g)
{
	if(g->allrecs > 1){
		return 2*g->edges/((double)g->allrecs*(g->allrecs - 1));
	}else{
		return 0;
	}
}
/******************************************************************************/
double closenessCentrality(Record* n, Graph* g)
{
	Record *r,*node;
	Edge *e;
	Properties *props;
	
	int distance;
	double sum = 0;
						
	Graph *visited;
	visited = create_Graph(2,3);
	List *queue;	
	create_list(&queue);
	
	// find distance from n to any other node
	
	props = create_Properties(1);
	setIntegerProperty(0,0,props);
	node = create_Node(n->id,props);			
	enqueue(&queue,node);				
	insertNode(node,visited);
			
	while(!empty(queue)){
		node = dequeue(&queue);

		r = lookupNode(node->id, visited);
		distance = getIntegerProperty(0,r->props);
		
		r = lookupNode(node->id,g);
		e = r->edges;
		
		while(e!=NULL){
			if(lookupNode(e->id, visited) == NULL){						
				props = create_Properties(1);
				setIntegerProperty(distance+1,0,props);
				node = create_Node(e->id,props);
				insertNode(node,visited);
				enqueue(&queue,node);	
				sum += (double)1/(distance+1);									
			}
			e = e->next;
		}
	}	

	destroyGraph3(&visited, 1, 0, -1, -1);
	destroy_list(&queue);
	
	if(g->allrecs > 1){
		return sum/(g->allrecs-1);
	}else{
		return 0;
	}		
}
/******************************************************************************/
double betweennessCentrality(Record* n, Graph* g)
{	
	Graph *visited = create_Graph(2,3);	
	Bucket *b;
	Record *rec,*r,*w,*u,*s;
	Edge *e;
	Properties *prop;		
	List *queue,*stack,*pred;	
	List_Node *search;
	
	int i,j,d1,d2,s1,s2;
	double sum=0,deltaw,deltau;	
	
	// Brande's algorithm 

	for(i=0;i<g->size;i++){
		b = g->ht[i];
		for(j=0;j<b->nrecs;j++){
			rec = b->cells[j];			
			if(rec->id != n->id){
					
				Graph *bet = create_Graph(2,3);							//using graph structure to save necessary info in properties
				create_list(&queue);									//queue for bfs
				create_list(&stack);									//stack to collect results	
				
				insertNode(rec,visited);
				
				prop = create_Properties(4);
				setIntegerProperty(0, 0,prop);							//distance from source to source is 1
				setIntegerProperty(1, 1,prop);							//shortest path from source to source is 1
				setDoubleProperty(0, 2,prop);							//dependecy for later accumulation				
				s = create_Node(rec->id,prop);
				insertNode(s,bet);						
				enqueue(&queue,s);					
				
				while(!empty(queue)){									//using bfs to find all shortest paths from s to every other node
					
					u = dequeue(&queue);				
					push(&stack,u);						
									
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
							if(u->id != rec->id){
								s2 = getIntegerProperty(1,u->props);									
								deltau = getDoubleProperty(2,u->props);							
								
								deltau = deltau + ((double)s2/s1 )*(1 + deltaw);	
								setDoubleProperty(deltau,2,u->props);																
							}			
							search = search->next;
						}						
					}
					
					if(w->id == n->id && w->id != rec->id){										
						sum += deltaw;
					}
				}				
				destroy_list(&queue);
				destroy_list(&stack);	
				destroyGraph3(&bet,3,0,3,-1);				
			}
		}
	}

	destroyGraph2(&visited);
	
	sum = sum/2;				//dependencies have been calculated two times	
	
	if(g->allrecs > 2){																
		return  sum /(((g->allrecs-2)*(g->allrecs-1)/2));
	}else{
		return 0;
	}	
}
/******************************************************************************/
