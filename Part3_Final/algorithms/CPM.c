#include "CPM.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************/
Cpm **computeCPMResults(Graph *graph, int *cliqueSize,int N)
{
	int i;
	Cpm **cpm = malloc(N*sizeof(Cpm*));
	// find top-N forums and their graphs
	ForumPopulation **topN = computeTopNForums(graph, N);
	Graph** topGraphs = computeTopNGraphs(graph, N);
	
	for(i=0; i<N; i++){
		if(topN[i]->id != -1){
			cpm[i] = malloc(sizeof(Cpm));
			cpm[i]->forum = malloc(sizeof(char)*(strlen(topN[i]->name)+1));
			strcpy(cpm[i]->forum, topN[i]->name);
			printf("\nForum %s\n\n",topN[i]->name);
			cpm[i]->clique3 = (cliquePercolationMethod(cliqueSize[0],topGraphs[i]));
			cpm[i]->clique4 = (cliquePercolationMethod(cliqueSize[1],topGraphs[i]));	
			
			free(topN[i]->name);
			destroyGraph3(&topGraphs[i], 0, 1, -1, -1);
		}else{
			cpm[i] = NULL;
		}
        free(topN[i]);
	}
	
	free(topN);
	free(topGraphs);
	return cpm;
}
/******************************************************************************/
int explore(Treenode* treenode,Graph *clique,Graph *neighbors, Graph *visited,List *results,int level,int k)
{
	Edge *e;
	Record *r,*newRec;
	List *list;
	
	
	if(level > k){		//a k-clique was discovered
		enqueue(&results,treenode);
		return 0;
	}
	
		
	r = lookupNode(treenode->id,clique);
	insertNode(r,treenode->visited);
	e = r->edges;
	
	while(e!=NULL){
		
		if(lookupNode(e->id, neighbors) != NULL && lookupNode(e->id, visited) == NULL){
						
			if(treenode->parent == NULL || (newRec = lookupNode(e->id,treenode->parent->visited)) == NULL){
			
				Treenode *child = malloc(sizeof(Treenode));
						
				create_list(&list);
				child->parent = treenode;
				child->children = list;
				child->visited = create_Graph(2,3);
				child->id = e->id;			
				enqueue(&treenode->children,child);

				explore(child,clique,neighbors,visited,results,level+1,k);				
			}else{		
				insertNode(newRec,treenode->visited);			// inherit parents visited
			}
			
		}
		e=e->next;
	}
	
	if(treenode->parent != NULL){
		insertNode(r,treenode->parent->visited);
	}
	
	return 1;	
}
/******************************************************************************/
Communities *cliquePercolationMethod(int k, Graph *g)
{
	int i,j;
	
	Graph *visited, *clique,*allcliques;
	Bucket *b,*bb,*bbb;
	Record *r,*newRec,*hypernode,*cliquemember;
	Edge *e,*newEdge;
	List  *results;
	
	create_list(&results);						
	visited = create_Graph(2,3);
	clique = create_Graph(2,3);
	allcliques = create_Graph(2,3);
	
	// create graph with nodes of k-1 degree
	
	for(i=0; i<g->size; i++){
		b = g->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];			
			if((r->degree/2) >= k-1){
				newRec = create_Node(r->id,NULL);
				insertNode(newRec, clique);
			}
		}
	}
	
	// create edges for previous graph
	
	for(i=0; i<clique->size; i++){
		b = clique->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];
			
			r = lookupNode(r->id,g);
			e = r->edges;
			while(e!=NULL){
				if(lookupNode(e->id, clique)){
					newEdge = setEdgeProperties(r->id, e->id, getStringProperty(0, e->props));
					insertEdge(r->id, newEdge, clique);
				}
				e = e->next;
			}
		}
	}
	
	int counter = 0;
	Treenode **trees = malloc(clique->allrecs * sizeof(Treenode *));
	
	for(i=0; i<clique->size; i++){
		b = clique->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];

			Treenode *tree;
			List *list1;
			create_list(&list1);
			
			tree = malloc(sizeof(Treenode));
			tree->id = r->id;
			tree->children = list1;
			tree->visited = create_Graph(2,3);
			tree->parent = NULL;				
			
			Graph *neighbors,*neighbors2;
			neighbors2 = create_Graph(2,3);
			neighbors = create_Graph(2,3);
			e = r->edges;			
			
			while(e!=NULL){
				if(lookupNode(e->id,visited)==NULL){
					newRec = lookupNode(e->id,clique);
					insertNode(newRec,neighbors2);		// find neighbors				
				}	
				e = e->next;
			}
			
			int x,y;
			
			for(x=0;x<neighbors2->size;x++){
				bb = neighbors2->ht[x];
				for(y=0;y<bb->nrecs;y++){
					newRec = bb->cells[y];
					int c = 0;
					e = newRec->edges ;
					while(e!=NULL){
						if(lookupNode(e->id,neighbors2)!=NULL){
							c++;
						}
						e = e->next;
					}
					if(c >= k-2){	// keep neighbors who know at least k-2 more
						insertNode(newRec,neighbors);	 
					}
				}
			}
			destroyGraph2(&neighbors2);		
						
			insertNode(r,visited);
			// explore all possible trees up to level =  k
			explore(tree,clique,neighbors,visited,results,2,k);
			
			trees[counter] = tree;
			counter++;
			
			destroyGraph2(&neighbors);
		}
	}
	
	destroyGraph2(&visited);
	destroyGraph3(&clique, 0, 1, -1, -1);
	
	//find all k-cliques and create graph with hypernodes
	
	int communityid = 0;
	int cliqueid = 0;
	
	List_Node *search = results->first;
	Treenode *node,*backtrack;
	
	while(search!=NULL){
		node = search->node;
		backtrack = node;
		
		hypernode = create_Node(cliqueid, NULL);
		hypernode->generic = create_Graph(2,3);		
		printf("Clique of size %d\t( ",k);
		while(backtrack!=NULL){
			printf("%d ",backtrack->id);
			cliquemember = create_Node(backtrack->id, NULL);
			insertNode(cliquemember, hypernode ->generic);		
			backtrack = backtrack->parent;			
		}	
		printf(")\n");
		
		insertNode(hypernode,allcliques);
		
		search = search->next;
		cliqueid++;
	}

	destroyAllTrees(&trees, counter);
	destroy_list(&results);
	
	// create edges for previous graph 
	
	visited = create_Graph(2,3);
	
	int m,n,x,y;
	for(i=0; i<allcliques->size; i++){
		b = allcliques->ht[i];
		for(j=0; j<b->nrecs; j++){
			hypernode = b->cells[j];	// get one hypernode
			insertNode(hypernode, visited);
			
			for(x=0; x<allcliques->size; x++){
				bbb = allcliques->ht[x];
				for(y=0; y<bbb->nrecs; y++){
					r = bbb->cells[y];	// get another hypernode
					if(lookupNode(r->id, visited) != NULL){
						continue;
					}
					counter=0;
			
					for(m=0; m<hypernode->generic->size; m++){
						bb = hypernode->generic->ht[m];
						for(n=0; n<bb->nrecs; n++){
							// get every member from first chosen hypernode
							cliquemember = bb->cells[n];
							
							if(lookupNode(cliquemember->id, r->generic) == NULL){
								counter++;
							}
							if(counter == 2){
								break;
							}
						}
						if(counter == 2){
							break;
						}
					}
					if(counter < 2){
						newEdge = create_Edge(hypernode->id, r->id, NULL);
						insertEdge(hypernode->id, newEdge, allcliques);
						newEdge = create_Edge(r->id, hypernode->id, NULL);
						insertEdge(r->id, newEdge, allcliques);
					}
					
				}
			}
		}
	}
	
	destroyGraph2(&visited);
	
	// find all communities (connected components) and their members
	
	Community *cc;
	Communities *cpmResults;
	cpmResults = malloc(sizeof(Communities));
	create_list(&cpmResults->communities);
	cpmResults->numberOfCommunities = 0;
	visited = create_Graph(2,3);
	
	for(i=0; i<allcliques->size; i++){
		b = allcliques->ht[i];
		for(j=0; j<b->nrecs; j++){
			hypernode = b->cells[j];		
			if(lookupNode(hypernode->id, visited) == NULL){
				cc = getCC(allcliques, visited, hypernode, communityid);
				enqueue(&cpmResults->communities, cc);
				communityid++;
				cpmResults->numberOfCommunities++;
			}
			
		}
	}
	
	destroyGraph2(&visited);
	destroyGraph3(&allcliques, 0, 0, -1, 0);
	
	return cpmResults;
}
/******************************************************************************/
Community* getCC(Graph *allcliques, Graph *visited, Record *hypernode, int communityid)
{	
	int i,j;
	Community *cc = NULL;
	
	List *queue;	
	create_list(&queue);
	
	Bucket *b;
	Record *r,*cliquemember;
	Edge *e;	
	
	// using bfs all connected components can be explored		
		
	insertNode(hypernode,visited);
	
	cc = malloc(sizeof(Community));	
	cc->members = create_Graph(2,3);
	cc->id = communityid;
	
	enqueue(&queue,hypernode);
	
								
	while(!empty(queue)){
		hypernode = dequeue(&queue);
		r = lookupNode(hypernode->id,allcliques);
		
		for(i=0; i<r->generic->size; i++){
			b = r->generic->ht[i];
			for(j=0; j<b->nrecs; j++){
				cliquemember = b->cells[j];
				if(lookupNode(cliquemember->id, cc->members) == NULL){
					cliquemember = create_Node(cliquemember->id, NULL);
					insertNode(cliquemember, cc->members);
				}
			}
		}
		
		e = r->edges;
		while(e!=NULL){
			if(lookupNode(e->id, visited) == NULL){							
				r = lookupNode(e->id,allcliques);
				insertNode(r,visited);
				enqueue(&queue,r);
			}
			e = e->next;
		}
	}

	destroy_list(&queue);
	
	return cc;
}
/******************************************************************************/
void destroyAllTrees(Treenode ***trees, int counter)
{
	int i;
	Treenode *t;
	for(i=0; i<counter; i++){
		t = (*trees)[i];
		destroyTree(&t);
	}
	free(*trees);
	*trees = NULL;
}
/******************************************************************************/
void destroyTree(Treenode **tree){
	List_Node *child;
	Treenode *t;
	child = (*tree)->children->first;
	
	while(child != NULL){
		t = child->node;
		destroyTree(&t);
		child = child->next;
	}
	
	destroyGraph2(&((*tree)->visited));
	destroy_list(&((*tree)->children));
	free(*tree);
	*tree = NULL;
}
/******************************************************************************/
