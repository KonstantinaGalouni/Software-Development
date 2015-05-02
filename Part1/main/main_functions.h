#ifndef MAIN_FUNCTIONS_H_
#define MAIN_FUNCTIONS_H_
#include "../graph/graph.h"

Record* create_Node(int id,Properties *p);
Record* setPersonProperties(int id, char* name, char* surname, int age);
void printPersonProperties(Record *r);

Edge* create_Edge(int start ,int end , Properties *p);
Edge* setEdgeProperties(int start, int end, char *property, int weight);

Properties* create_Properties(int size);
void setStringProperty(char *property ,int index,Properties *p);
void setIntegerProperty(int property, int index, Properties *p);
char* getStringProperty(int index, Properties* p);
int getIntegerProperty(int index, Properties* p);

#endif
