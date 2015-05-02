#ifndef MAIN_FUNCTIONS_H_
#define MAIN_FUNCTIONS_H_
#include "../graph/graph.h"

Record* create_Node(int id,Properties *p);
Record* setPersonProperties(int id, char* firstName, char* lastName, char* gender,
							char* birthday, char* creationDate, char* locationIp,
						    char* browserUsed);
void printPersonProperties(Record *r);

Edge* create_Edge(int start ,int end , Properties *p);
Edge* setEdgeProperties(int start, int end, char *property);
Edge* setWeightedEdgeProperties(int start, int end, char *property, double weight);

Properties* create_Properties(int size);
void setStringProperty(char *property ,int index,Properties *p);
void setIntegerProperty(int property, int index, Properties *p);
void setDoubleProperty(double property, int index, Properties *p);
void setGraphProperty(int property, int index, int numProps, Properties *p);
char* getStringProperty(int index, Properties* p);
int getIntegerProperty(int index, Properties* p);
double getDoubleProperty(int index, Properties* p);
Graph* getGraphProperty(int index, Properties *p);

int sortInterests(int *i , int newid, int num);

Graph* graph_tag();
Graph* graph_person(int bucketsNumber, int bucketSize);
Graph* graph_post_has_creator();
Graph* graph_person_isLocatedIn_place();
Graph* graph_person_workAt_organisation();
Graph* graph_person_studyAt_organisation();
Graph* graph_forum_containerOf_post_has_creator();

void update_person_hasInterest_tag(Graph *g);
void update_person_knows_person(Graph *g);
void update_person_likes_post(Graph *g,Graph *posts);
void update_forum_has_member(Graph *g);
void update_comment_replyOf_post(Graph *g,Graph *posts);

#endif
