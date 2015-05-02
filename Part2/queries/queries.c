#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queries.h"
#include "../list/list.h"

Matches* matchSuggestion(Record* n, int k, int h, int x, int limit, Graph* g)
{
	Matches *matches = malloc(limit*sizeof(Matches));

	int i,j,insert,index1,index2,index=0,tomh=0,enwsh=0;
	double score=0.0;
	char *gender1,*gender2,*age;
	Bucket *b;
	Record *r;
	Edge *e;
	Graph *location, *work, *study;
	
	if(n == NULL){
		printf("Node was not found in graph!\n");
		return matches;
	}
	
	update_person_hasInterest_tag(g);
	location = graph_person_isLocatedIn_place();
	work = graph_person_workAt_organisation();
	study = graph_person_studyAt_organisation();
	
	for(i=index;i<limit;i++){	
		matches[i].id = -1;
		matches[i].score = -1;
	}

	for(i=0; i<g->size; i++){
		b = g->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];
			if(getStringProperty(3, n->props) != NULL){
				index2 = atoi(strtok(getStringProperty(3, n->props), "-"));
			}
			insert = 1;

			gender1 = getStringProperty(2, r->props);
			gender2 = getStringProperty(2, n->props);
			if(gender1!=NULL && gender2!=NULL && strcmp(gender1,gender2) == 0){
				//same gender
				continue;
			}

			if(insert == 1){
				age = strtok(getStringProperty(3, r->props), "-");
				if(age == NULL){
					continue;
				}
				index1 = atoi(age);

				if(index1 - index2 > x || index2 - index1 > x){	//more than x ages
					continue;
				}
			}
			
			if(insert == 1){		//check interests
				tomh = 0;
				enwsh = 0;

				if(n->interests == NULL || r->interests == NULL){
					insert = 0;
				}else{
					int i,j;
					Record *tempR;
					Bucket *tempB;

					for(i=0; i<n->interests->size; i++){
						tempB = n->interests->ht[i];
						for(j=0; j<tempB->nrecs; j++){
							tempR = tempB->cells[j];
							if(lookupNode(tempR->id, r->interests) != NULL){
								tomh++;
							}
							enwsh++;
						}
					}
					enwsh += r->interests->allrecs - tomh;
				}

				if(tomh >= k){	//more than k same interests
					insert = 1;
				}else{			//less than or equal to k same interests
					insert = 0;
				}
			}
			if(insert == 1){
				if(reachNode1_simpeBFS(r->id, n->id, g) > h ){	//more than h steps
					insert = 0;
				}
			}

			if(insert == 1){
				e = r->edges;
				while(e != NULL){		//for all edges of record r
					if(e->id == n->id){
						//friends
						insert = 0;
						break;
					}
					e = e->next;
				}
			}

			if(insert == 1){		//check location
				Record *temp1 = lookupNode(n->id, location);
				int loc1 = getIntegerProperty(0, temp1->props);
				Record *temp2 = lookupNode(r->id, location);
				int loc2 = getIntegerProperty(0, temp2->props);

				if(loc1 != loc2){
					insert = 0;
				}

				if(insert == 0){	//check study
					temp1 = lookupNode(n->id, study);
					temp2 = lookupNode(r->id, study);
					if(temp1 != NULL && temp2 != NULL){
						int m,l;
						Record *r;
						Bucket *b;
						Graph *g1 = temp1->generic;
						Graph *g2 = temp2->generic;
						
						for(m=0; m<g1->size; m++){
							b = g1->ht[m];
							for(l=0; l<b->nrecs; l++){
								r = b->cells[l];
								if(lookupNode(r->id, g2) != NULL){
									insert = 1;
									break;
								}
							}
							if(insert == 1){
								break;
							}
						}
					}
				}
				
				if(insert == 0){	//check work
					temp1 = lookupNode(n->id, work);
					temp2 = lookupNode(r->id, work);
					if(temp1 != NULL && temp2 != NULL){
						int m,l;
						Record *r;
						Bucket *b;
						Graph *g1 = temp1->generic;
						Graph *g2 = temp2->generic;
						for(m=0; m<g1->size; m++){
							b = g1->ht[m];
							for(l=0; l<b->nrecs; l++){
								r = b->cells[l];
								if(lookupNode(r->id, g2) != NULL){
									insert = 1;
									break;
								}
							}
							if(insert == 1){
								break;
							}
						}
					}
				}
			}

			if(insert == 1){			//insert record in matches
				score = tomh/(double)enwsh;
				insert = sortMatches(matches ,score, limit);
				if(insert < limit){
					matches[insert].id = r->id;
					matches[insert].score = score;
				}
				index++;				
			}
		}
	}
	
	for(i=0; i<limit; i++){
		if(matches[i].score != -1){
			matches[i].score = 1-matches[i].score;
		}
	}
	
	destroyGraph3(&location, 1,0,-1,-1);
	destroyGraph3(&work, 1,0,-1,0);
	destroyGraph3(&study, 1,0,-1,0);

	return matches;
}
/******************************************************************************/
Graph* getTopStalkers(int k, int x, int centralityMode, Graph* g,Stalkers* st)
{	
	Graph *friends,*posts;	
	Graph *stalkers = create_Graph(2,3);
	Graph *current = create_Graph(2,3);
	Bucket *bucket,*b;
	Record *rec,*r;
	Edge *e,*edge;	
	Properties *props;
	
	int i,j,m,n,l;	
	double score;	
	
	posts = graph_forum_containerOf_post_has_creator();	
	update_forum_has_member(g);	
	update_person_likes_post(g,posts);	
	

	for(i=0;i<k;i++){
		st[i].id = -1;
		st[i].score = -1;
	}
	
	for(i=0;i<g->size;i++){
		bucket = g->ht[i];
		for(j=0;j<bucket->nrecs;j++){			
			rec = bucket->cells[j];
			e = rec->edges;
			friends = create_Graph(2,3);
			
			//find all friends of record
			
			while(e!=NULL){							
				r = lookupNode(e->id,g);
				insertNode(r,friends);					
				e = e->next;
			}					
			
			//check if target is not a friend and then check his/her likes
			
			if(rec->likes != NULL){
				for(m=0;m<rec->likes->size;m++){				
					b = rec->likes->ht[m];
					for(n=0;n<b->nrecs;n++){
						r = b->cells[n];	
						if(lookupNode(r->id,friends) == NULL && r->id != rec->id){
							l = getIntegerProperty(0,r->props);
							if(l > x){																					
								insertNode(rec,current);	//find all stalkers																
							}
						}
					}
				}
			}
			destroyGraph2(&friends);
		}
	}	
	
	//create stalkers graph
	
	for(i=0;i<current->size;i++){
		b = current->ht[i];
		for(j=0;j<b->nrecs;j++){
			rec = b->cells[j];			
			
			e = rec->edges;								
			rec = create_Node(rec->id,NULL);			
			insertNode(rec,stalkers);	
				
			while(e!=NULL){				
				if((r = lookupNode(e->id,current))!=NULL){					
					props = create_Properties(1);
					setStringProperty("knows",0,props);					
					edge = create_Edge(0,e->id,props);													
					insertEdge(rec->id,edge,stalkers);
				}				
				e = e->next;
			}
		}
	}

	// calculate centrality and sort k-top stalkers		
			
	for(i=0;i<stalkers->size;i++){
		b = stalkers->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];		
			if(centralityMode == 1){
				score = closenessCentrality(r,stalkers);
			}else{
				score = betweennessCentrality(r,stalkers);
			}						
			
			m = sortMatches(st,score,k);
			
			if(m < k){
				st[m].id = r->id;
				st[m].score = score;
			}
		}
	}
	
	// print k-top stalkers with their forums
	
	for(i=0;i<k;i++){
		if(st[i].score == -1){
			break;
		}
		r = lookupNode(st[i].id,g);
		if(r->forums == NULL){
			printf("Stalker ID = %4d | Forum ID --\n",r->id);
		}else{
			for(m=0;m<r->forums->size;m++){
				b = r->forums->ht[m];				
				for(n=0;n<b->nrecs;n++){
					rec = b->cells[n];
				//	printf("Stalker ID = %4d | Forum ID %d\n",r->id,rec->id);
				}
			}
		}		
	}

	destroyGraph2(&current);	
	destroyGraph3(&posts,2,0,-1,-1);
	
	return stalkers;
}
/******************************************************************************/
void replace(char **trend,int index,int k,char *buffer)
{
	char previous[90];

	if(index < k-1){
		strcpy(previous,buffer);
		strcpy(buffer,trend[index+1]);
		strcpy(trend[index+1],previous);
		replace(trend,index+1,k,buffer);				
	}
}
/******************************************************************************/
void findTrends(int k, Graph *g, char **womenTrends, char **menTrends)
{
	int i,j,max,index,*m,*w;
	char buffer[90];
	Bucket *b;
	Record *r, *newRec,*tag;
	Edge *e, *newEdge;

	Graph *women = create_Graph(2,3);
	Graph *men = create_Graph(2,3);
	Graph *tags = graph_tag(g);
	
	update_person_hasInterest_tag(g);
	
	for(i=0;i<k;i++){	
		menTrends[i] = malloc(sizeof(char)*90);		
		womenTrends[i] = malloc(sizeof(char)*90);	
		strcpy(menTrends[i],"");
		strcpy(womenTrends[i],"");	
	}
	
	for(i=0; i<g->size; i++){
		b = g->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];			
			if(getStringProperty(2, r->props)!= NULL){			
				newRec = create_Node(r->id,NULL);
				newRec->interests = r->interests;
				e = r->edges;
				
				//men's Graph
				
				if(strcmp(getStringProperty(2, r->props), "male") == 0){
				
					insertNode(newRec,men);
					while(e != NULL){
						if((r = lookupNode(e->id, g)) != NULL && getStringProperty(2, r->props)!= NULL ){
							if(strcmp(getStringProperty(2, r->props), "male") == 0){
								newEdge = setEdgeProperties(newRec->id, e->id, getStringProperty(0, e->props));
								insertEdge(newRec->id, newEdge, men);
							}
						}
						e = e->next;
					}
				}else{
					
				//women's Graph
					
					insertNode(newRec,women);
					while(e != NULL){
						if((r = lookupNode(e->id, g)) != NULL && getStringProperty(2, r->props)!= NULL ){
							if( strcmp(getStringProperty(2, r->props), "female") == 0){
								newEdge = setEdgeProperties(newRec->id, e->id, getStringProperty(0, e->props));
								insertEdge(newRec->id, newEdge, women);
							}
						}
						e = e->next;
					}
				}
			}
		}
	}

	m = malloc(sizeof(int)*k);
	w = malloc(sizeof(int)*k);

	memset(m,-1,k*sizeof(int));
	memset(w,-1,k*sizeof(int));

	for(i=0;i<tags->size;i++){
		b = tags->ht[i];
		for(j=0;j<b->nrecs;j++){
			tag = b->cells[j];

			max = find_maxCC(g,men,tag,0);			
			index = sortTrends(m,max,k);
			
			if(max > 1 && index < k ){				
				m[index] = max;				
				strcpy(buffer,menTrends[index]);
				replace(menTrends,index,k,buffer);				
				strcpy(menTrends[index], getStringProperty(2, tag->props));
			}			

			max = find_maxCC(g,women,tag,1);			
			index = sortTrends(w,max,k);

			if(max > 1 && index < k){
				w[index] = max;								
				strcpy(buffer,womenTrends[index]);
				replace(womenTrends,index,k,buffer);
				strcpy(womenTrends[index], getStringProperty(2, tag->props));
			}
		}
	}	
	
	int x=0,y=0;
	
	for(i = k-1;i>=0 && (x==0 || y==0);i--){
		if(strcmp("",menTrends[i])==0){
			free(menTrends[i]);
			menTrends[i] = NULL;
		}else{
			x = 1;
		}
		if(strcmp("",womenTrends[i])==0){
			free(womenTrends[i]);
			womenTrends[i] = NULL;
		}else{
			y = 1;
		}
	}	
	
	free(m);
	free(w);

	destroyGraph3(&men,0,1,-1,-1);
	destroyGraph3(&women,0,1,-1,-1);
	destroyGraph3(&tags,3,0,-1,-1);
}
/******************************************************************************/
Graph* buildTrustGraph(int forumID, Graph *g)
{	
	int i,j,m,n,likes,comments,totalLikes, totalComments;
	double weight;
	
	Bucket *b,*bucket;
	Record *r,*newRec, *sourceforum,*targetforum,*source,*target;
	Edge *newEdge;
	Graph *posts;
	Graph *trustGraph = create_Graph(2,3);
	
	posts = graph_forum_containerOf_post_has_creator();
	update_forum_has_member(g);
	update_person_likes_post(g,posts);
	update_comment_replyOf_post(g,posts);
	
	// create graph with the members of forumID
	
	for(i=0; i<g->size; i++){
		b = g->ht[i];
		for(j=0; j<b->nrecs; j++){
			r = b->cells[j];
			if(r->forums!=NULL &&  lookupNode(forumID, r->forums)!= NULL){
				newRec = create_Node(r->id,NULL);
				insertNode(newRec,trustGraph);
			}
		}
	}
	
	// create trust graph
	
	for(i=0; i<trustGraph->size; i++){
		bucket = trustGraph->ht[i];
		for(j=0; j<bucket->nrecs; j++){
				
			source = bucket->cells[j];
			source = lookupNode(source->id,g);
			
			// for each member calculate all weighted edges on other members
						
			if(source->likes != NULL){
				for(m=0; m<trustGraph->size; m++){
					b = trustGraph->ht[m];
					for(n=0; n<b->nrecs; n++){
						target = b->cells[n];
						
						if(source->id == target->id){
							continue;
						}
						
						likes = 0;
						comments = 0;
						weight = 0;
						
						// check if source member has liked target member
						
						if((target = lookupNode(target->id, source->likes)) != NULL){
							
							// check if source member has liked forumID	
											
							if((sourceforum = lookupNode(forumID, source->forums)) != NULL){
								
								// check if likes on target were made in the forum
								
								if((targetforum = lookupNode(forumID,target->generic))!=NULL){
									
									likes = getIntegerProperty(0, targetforum->props);
									totalLikes = getIntegerProperty(0,sourceforum->props);
								
									if(totalLikes > 0){
										weight = 0.3*likes/(double)totalLikes;
									}
									
									comments = getIntegerProperty(1,targetforum->props);
									totalComments = getIntegerProperty(1,sourceforum->props);
									if(totalComments > 0){
										weight += 0.7*comments/(double)totalComments;
									}
								}								
							}
						}

						if(weight > 0){
							newEdge = setWeightedEdgeProperties(source->id, target->id, "trust", weight);
							insertEdge(source->id, newEdge, trustGraph);
						}
					}
				}
			}
		}
	}

	destroyGraph3(&posts,2,0,-1,-1);
	
	return trustGraph;
}
/******************************************************************************/
double estimateTrust(Record *a, Record *b, Graph *trustGraph)
{
	int d=0;
	double trustEdge,trust=1;
	List *queue;
	Properties *prop;
	Record *node, *r;
	Edge *e;	
	Graph *visited;
	
	if(a == NULL || b == NULL || trustGraph->allrecs == 0){	
		return 0;
	}
	
	visited = create_Graph(2, 3);
	create_list(&queue);

	prop = create_Properties(2);
	setIntegerProperty(d, 0, prop);			//distance
	setDoubleProperty(trust, 1, prop);		//trust
	node = create_Node(a->id, prop);
	insertNode(node, visited);

	enqueue(&queue,node);
	while(node->id != b->id){
		node = dequeue(&queue);

		if(node == NULL){		//empty queue
			trust = 0;
			break;
		}

		r = lookupNode(node->id,trustGraph);

		if(r == NULL){
			printf("There is no record with id = %d\n", node->id);
			trust = 0;
			break;			
		}

		e = r->edges;

		d = getIntegerProperty(0, node->props);		//current distance
		trust = getDoubleProperty(1, node->props);	//current trust

		while(e != NULL){
			trustEdge = getDoubleProperty(1,e->props);
			if((r = lookupNode(e->id, visited)) == NULL){

				prop = create_Properties(2);
				setIntegerProperty(d+1, 0, prop);
				setDoubleProperty(trust*trustEdge, 1, prop);
				r = create_Node(e->id, prop);
				insertNode(r , visited);
				enqueue(&queue,r);
			}else{
				//if exists in visited and distance is the same, trust is bigger then change
				if(getIntegerProperty(0, r->props) == d+1 && getDoubleProperty(1, r->props) < trust*trustEdge){
					setDoubleProperty(trust*trustEdge, 1, r->props);
				}
			}
			e = e->next;
		}
	}

	destroyGraph3(&visited,2,2,-1,-1);
	destroy_list(&queue);

	return trust;
}
/******************************************************************************/
int sortMatches(Stalkers *st,double score,int k)
{	
	int l,r,m;	
	
	r = k - 1;
	l = 0;
	
	m = r/2;
	while(r > l){		
		if(st[m].score < score){
			r = m ;
		}else {
			l = m + 1;
		}
		m = l + ((r-l)/2);
	}
	
	if(st[m].score > score){
		return k;
	}
	
	memmove(&(st[m+1]),&(st[m]),sizeof(Stalkers)*(k-1-m));
	return m;
}
/******************************************************************************/
int sortTrends(int *st,int score,int k)
{
	int l,r,m;

	r = k - 1;
	l = 0;

	m = r/2;
	while(r > l){
		if(st[m] < score){
			r = m ;
		}else {
			l = m + 1;
		}
		m = l + ((r-l)/2);
	}
	
	if(st[m] > score){
		return k;
	}
	
	memmove(&(st[m+1]),&(st[m]),sizeof(int)*(k-1-m));
	return m;
}
/******************************************************************************/
int find_maxCC(Graph *g,Graph *gender,Record *tag,int index)
{	
	int i,j,p;		
	Bucket *b;
	Record *r;
	Edge *e,*newEdge;
	Properties *props;
	Graph *tagCC = create_Graph(2,3);
	
	for(i=0;i<gender->size;i++){
		b = gender->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];		
			if(lookupNode(tag->id,r->interests) != NULL){
				r = create_Node(r->id,NULL);
				insertNode(r,tagCC);
			}
		}
	}

	for(i=0;i<tagCC->size;i++){
		b = tagCC->ht[i];
		for(j=0;j<b->nrecs;j++){
			r = b->cells[j];
			r = lookupNode(r->id,g);
			e = r->edges;
			
			while(e!=NULL){
				if(lookupNode(e->id,tagCC) != NULL ){
					props = create_Properties(1);
					setStringProperty("knows",0,props);
					newEdge = create_Edge(0,e->id,props);
					insertEdge(r->id,newEdge,tagCC);
				}
				e = e->next;
			}
		}
	}

	p = maxCC(tagCC);

	setIntegerProperty(p,index,tag->props);
	
	destroyGraph3(&tagCC,0,1,-1,-1);
	
	return p;
}
/******************************************************************************/
