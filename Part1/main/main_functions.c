#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main_functions.h"

/******************************************************************************/
Record* create_Node(int id,Properties *p)
{
	Record *r = malloc(sizeof(Record));	

	r->id = id;
	r->props  = p;
	r->edges = NULL;
	
	return r;
}
/******************************************************************************/
Record* setPersonProperties(int id, char* name, char* surname, int age)
{
    /*create properties*/
    Properties* prop = create_Properties(PERSON_PROPERTIES_NUM);
    if(name != NULL) { 
		setStringProperty(name, 0, prop);		//record's first property -> string
	}
    if(surname != NULL) {
		setStringProperty(surname, 1, prop);	//second property -> string
	}
    setIntegerProperty(age, 2, prop);			//third property ->integer

    /*create node*/
    Record* n = create_Node(id, prop);

    return n;
}
/******************************************************************************/
void printPersonProperties(Record *r)
{
	Properties *p = r->props;
	printf("\n---Person Properties---\n");
	printf("   id = %d\n", r->id);
	if(getStringProperty(0, p) != NULL){
		printf("   name = %s\n", getStringProperty(0, p));
	}
	if(getStringProperty(1, p) != NULL){
		printf("   surname = %s\n", getStringProperty(1, p));
	}
	printf("   age = %d\n\n", getIntegerProperty(2, p));
}
/******************************************************************************/
Edge* create_Edge(int start ,int end , Properties *p)
{
	Edge *e;
	
	e = malloc(sizeof(Edge));
	e->props = p;
	e->id = end;	
	e->next = NULL;
	
	return e;
}
/******************************************************************************/
Edge* setEdgeProperties(int start, int end, char *property, int weight) {

    /*create edge properties*/
    Properties* p = create_Properties(PERSON_REL_PROPERTIES_NUM);
    setStringProperty(property, 0, p);		//edge's first property -> string
    setIntegerProperty(weight, 1, p);		//second property -> integer

    /*create an edge*/
    Edge* e = create_Edge(start, end, p);
    return e;
}
/******************************************************************************/
Properties* create_Properties(int size)
{
	int i;

	Properties *p = malloc(sizeof(Properties));	
	p->properties = malloc(sizeof(void*)*size);
	
	for(i=0;i<size;i++){
		p->properties[i] = NULL;
	}
	
	return p;
}
/******************************************************************************/
void setStringProperty(char *property ,int index,Properties *p)
{
	p->properties[index] = malloc(strlen(property)+1);
	strcpy(p->properties[index],property);
}
/******************************************************************************/
void setIntegerProperty(int property, int index, Properties *p)
{
	p->properties[index] = malloc(sizeof(int));
	*(int*)p->properties[index] = property;
}
/******************************************************************************/
char* getStringProperty(int index, Properties* p)
{
	return p->properties[index];
}
/******************************************************************************/
int getIntegerProperty(int index, Properties* p)
{
	return *((int*)p->properties[index]);
}
/******************************************************************************/