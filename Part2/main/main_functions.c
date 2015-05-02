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
    r->degree = 0;
    r->interests = NULL;
    r->likes = NULL;
    r->forums = NULL;
    r->generic = NULL;

    return r;
}
/******************************************************************************/
Record* setPersonProperties(int id, char* firstName, char* lastName, char* gender,
        char* birthday, char* creationDate, char* locationIp, char* browserUsed)
{
    /*create properties*/
    Properties* prop = create_Properties(PERSON_PROPERTIES_NUM);

    if(firstName != NULL) {
        setStringProperty(firstName, 0, prop);
    }
    if(lastName != NULL) {
        setStringProperty(lastName, 1, prop);
    }
    if(gender != NULL) {
        setStringProperty(gender, 2, prop);
    }
    if(birthday != NULL) {
        setStringProperty(birthday, 3, prop);
    }
    if(creationDate != NULL) {
        setStringProperty(creationDate, 4, prop);
    }
    if(locationIp != NULL) {
        setStringProperty(locationIp, 5, prop);
    }
    if(browserUsed != NULL) {
        setStringProperty(browserUsed, 6, prop);
    }

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
Edge* setEdgeProperties(int start, int end, char *property) {

    /*create edge properties*/
    Properties* p = create_Properties(PERSON_REL_PROPERTIES_NUM);
    setStringProperty(property, 0, p);        //edge's first property -> string

    /*create an edge*/
    Edge* e = create_Edge(start, end, p);
    return e;
}
/******************************************************************************/
Edge* setWeightedEdgeProperties(int start, int end, char *property, double weight) {

    //create edge properties
    Properties* p = create_Properties(2);
    setStringProperty(property, 0, p);        //edge's first property -> string
    setDoubleProperty(weight, 1, p);

    //create an edge
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
    if(p->properties[index] == NULL){
        p->properties[index] = malloc(strlen(property)+1);
    }
    strcpy(p->properties[index],property);
}
/******************************************************************************/
void setIntegerProperty(int property, int index, Properties *p)
{
    if(p->properties[index] == NULL){
        p->properties[index] = malloc(sizeof(int));
    }
    *(int*)p->properties[index] = property;
}
/******************************************************************************/
void setDoubleProperty(double property, int index, Properties *p)
{
    if(p->properties[index] == NULL){
        p->properties[index] = malloc(sizeof(double));
    }
    *(double*)p->properties[index] = property;
}
/******************************************************************************/
void setGraphProperty(int property, int index, int numProps, Properties *p)
{
    Properties *props;
    Graph *g;
    if(p->properties[index] == NULL){
        g = create_Graph(2,3);
        p->properties[index] = g;
    }else{
        g = getGraphProperty(index, p);
    }

    if(numProps == 0){
        props = NULL;
    }else{
        props = create_Properties(numProps);
    }
    Record *r = create_Node(property,props);
    insertNode(r, g);
}
/******************************************************************************/
Graph* getGraphProperty(int index, Properties *p){
    return ((Graph*)p->properties[index]);
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
double getDoubleProperty(int index, Properties* p)
{
    return *((double*)p->properties[index]);
}
/******************************************************************************/
Graph* graph_person(int bucketsNumber, int bucketSize)
{
    FILE *fp;
    int id;
    char *firstName, *lastName, *gender, *birthday;
    char *creationDate, *locationIP, *browserUsed;
    char line[90];
    Graph *g = create_Graph(bucketsNumber, bucketSize);
    Record *r;

    if((fp = fopen("dataset/person.csv","r")) == NULL){
        perror("File person does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]", line);

    while(fscanf(fp, "%d|%[^\n]", &id, line) == 2){
        firstName = strtok(line, "|");
        lastName = strtok(NULL, "|");
        gender = strtok(NULL, "|");
        birthday = strtok(NULL, "|");
        creationDate = strtok(NULL, "|");
        locationIP = strtok(NULL, "|");
        browserUsed = strtok(NULL, "\n");
        r = setPersonProperties(id, firstName, lastName, gender, birthday, creationDate, locationIP, browserUsed);
        insertNode(r,g);
    }
    fclose(fp);
    return g;
}
/******************************************************************************/
void update_person_knows_person(Graph *g)
{
    FILE *fp;
    char line[90];
    int id1,id2;
    Edge *e;

    if((fp = fopen("dataset/person_knows_person.csv","r")) == NULL){
        perror("File person_knows_person does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]", line);

    while(fscanf(fp, "%d|%d\n", &id1, &id2) == 2){
        e = setEdgeProperties(id1, id2, "knows");
        insertEdge(id1 , e, g);
    }
    fclose(fp);
}
/******************************************************************************/
Graph* graph_person_isLocatedIn_place()
{
    FILE *file;
    int person, loc;
    char line[50];
    Graph* location = create_Graph(2,3);
    Properties *props;
    Record *rec;

    if((file = fopen("dataset/person_isLocatedIn_place.csv","r")) == NULL){
        perror("File person_isLocatedIn_place does not exist!\n");
        exit(1);
    }

    fscanf(file, "%[^\n]", line);

    while(fscanf(file, "%d|%d\n", &person, &loc) == 2){
        props = create_Properties(1);
        setIntegerProperty(loc,0,props);
        rec = create_Node(person,props);
        insertNode(rec,location);
    }
    fclose(file);

    return location;
}
/******************************************************************************/
Graph* graph_person_workAt_organisation()
{
    FILE *file;
    int person, org, year;
    char line[50];
    Graph* work = create_Graph(2,3);
    Properties *props;
    Record *rec,*node;

    if((file = fopen("dataset/person_workAt_organisation.csv","r")) == NULL){
        perror("File person_workAt_organisation does not exist!\n");
        exit(1);
    }

    fscanf(file, "%[^\n]", line);

    while(fscanf(file, "%d|%d|%d\n", &person, &org, &year) == 3){
        if((rec = lookupNode(person,work)) == NULL){
            props = create_Properties(1);
            setIntegerProperty(year,0,props);
            rec = create_Node(person,props);
            rec->generic = create_Graph(2,3);
            insertNode(rec,work);
        }else if(getIntegerProperty(0,rec->props) < year){
            destroyGraph3(&(rec->generic),0,0,-1,-1);
            rec->generic = create_Graph(2,3);
            setIntegerProperty(year,0,rec->props);
        }else if(getIntegerProperty(0,rec->props) > year){
            continue;
        }
        node = create_Node(org,NULL);
        insertNode(node,rec->generic);
    }
    fclose(file);

    return work;
}
/******************************************************************************/
Graph* graph_person_studyAt_organisation()
{
    FILE *file;
    int person, org, year;
    char line[50];
    Graph* study = create_Graph(2,3);
    Properties *props;
    Record *rec,*node;

    if((file = fopen("dataset/person_studyAt_organisation.csv","r")) == NULL){
        perror("File person_studyAt_organisation does not exist!\n");
        exit(1);
    }

    fscanf(file, "%[^\n]", line);

    while(fscanf(file, "%d|%d|%d\n", &person, &org, &year) == 3){
        if((rec = lookupNode(person,study)) == NULL){
            props = create_Properties(1);
            setIntegerProperty(year,0,props);
            rec = create_Node(person,props);
            rec->generic = create_Graph(2,3);
            insertNode(rec,study);
        }else if(getIntegerProperty(0,rec->props) < year){
            destroyGraph3(&(rec->generic),0,0,-1,-1);
            rec->generic = create_Graph(2,3);
            setIntegerProperty(year,0,rec->props);
        }else if(getIntegerProperty(0,rec->props) > year){
            continue;
        }
        node = create_Node(org,NULL);
        insertNode(node,rec->generic);
    }
    fclose(file);

    return study;
}
/******************************************************************************/
void update_person_hasInterest_tag(Graph *g)
{
    int e1, e2,a=0;
    FILE *file;
    char line[50];
    Record *r1, *r2;

    if((file = fopen("dataset/person_hasInterest_tag.csv","r")) == NULL){
        perror("File person_hasInterest_tag does not exist!\n");
        exit(1);
    }
    fscanf(file, "%[^\n]", line);

    while(fscanf(file, "%d|%d\n", &e1, &e2) == 2){
        if((r1 = lookupNode(e1, g)) != NULL){
            if(r1->interests == NULL){
                r1->interests = create_Graph(2,3);
            }else if(a==0){
                break;
            }
            a=1;

            if((r2 = lookupNode(e2,r1->interests)) == NULL){
                r2 = create_Node(e2,NULL);
                insertNode(r2,r1->interests);
            }
        }
    }
    fclose(file);
}
/******************************************************************************/
Graph* graph_forum_containerOf_post_has_creator()
{
    FILE *fp;
    int postID,personID,forumID;
    char buffer[90];
    Graph *posts = create_Graph(2,3);
    Record *post;
    Properties *props;

    if((fp = fopen("dataset/post_hasCreator_person.csv","r")) == NULL){
        perror("File post_hasCreator_person does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);

    //create graph with records of postID and propeties creatorID, forumID

    while(fscanf(fp,"%d|%d",&postID,&personID) == 2){
		if(lookupNode(postID,posts) == NULL){
			props = create_Properties(2);
			setIntegerProperty(personID,0,props);
			setIntegerProperty(-1,1,props);
			post = create_Node(postID,props);
			insertNode(post,posts);
		}
    }
    fclose(fp);

    if((fp = fopen("dataset/forum_containerOf_post.csv","r")) == NULL){
        perror("File forum_containerOf_post does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);

    while(fscanf(fp,"%d|%d",&forumID,&postID) == 2){
        if((post = lookupNode(postID, posts)) != NULL){
            setIntegerProperty(forumID,1,post->props);
        }
    }

    fclose(fp);

    return posts;
}
/******************************************************************************/
void update_person_likes_post(Graph *g,Graph *posts)
{
    FILE *fp;
    int postID,personID,creatorID,forumID,likes,a=0;
    char buffer[90];
    Record *creator,*post,*forum,*person;
    Properties *props;

    if((fp = fopen("dataset/person_likes_post.csv","r")) == NULL){
        perror("File person_likes_post does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);

    //collect the number of likes a person has on his posts from each other person

    while(fscanf(fp,"%d|%d|%[^\n]",&personID,&postID,buffer) == 3){
        post = lookupNode(postID,posts);
        if(post == NULL){
            continue;
        }
        creatorID = getIntegerProperty(0,post->props);
        person  = lookupNode(personID,g);
        forumID = getIntegerProperty(1,post->props);
        
        if(forumID == -1){
            continue;
        }

        if(person!=NULL){
            if(person->likes == NULL){
                person->likes = create_Graph(2,3);
            }else if(a==0){
                break;
            }
            a=1;
        }else{
            continue;
        }
		
        if((creator = lookupNode(creatorID,person->likes)) == NULL){
			
            // insert the creator of post in the likes of the person

            props = create_Properties(1);                    // total likes on creator
            setIntegerProperty(1,0,props);
            creator = create_Node(creatorID,props);
            insertNode(creator,person->likes);
			
            // insert the forumID with the number of likes and replies on it

            creator->generic = create_Graph(2,3);            // forums
            props = create_Properties(2);
            setIntegerProperty(1,0,props);                    // likes
            setIntegerProperty(0,1,props);                    // replies
            forum = create_Node(forumID,props);
            insertNode(forum,creator->generic);
        }else{

            // increase likes on forum for this person
			
            likes = getIntegerProperty(0,creator->props);
            setIntegerProperty(likes+1,0,creator->props);

            if((forum = lookupNode(forumID, creator->generic)) == NULL){
                props = create_Properties(2);
                setIntegerProperty(1,0,props);
                setIntegerProperty(0,1,props);
                forum = create_Node(forumID,props);
                insertNode(forum,creator->generic);
            }else{
                likes = getIntegerProperty(0,forum->props);
                setIntegerProperty(likes+1,0,forum->props);
            }
        }

        // increase total likes on forum by person

        if(person->forums != NULL && (forum = lookupNode(forumID, person->forums)) != NULL){
            likes = getIntegerProperty(0, forum->props);
            setIntegerProperty(likes+1, 0, forum->props);
        }
    }
    fclose(fp);
}
/******************************************************************************/
void update_forum_has_member(Graph *g)
{
    FILE *fp;
    int forumID,personID,a=0;
    char buffer[90];
    Record *forum,*rec;
    Properties *props;

    if((fp = fopen("dataset/forum_hasMember_person.csv","r")) == NULL){
        perror("File forum_hasMember_person does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);
    while(fscanf(fp,"%d|%d|%[^\n]",&forumID,&personID,buffer) == 3){
        rec = lookupNode(personID,g);

        if(rec!=NULL){
            if(rec->forums == NULL){
                rec->forums = create_Graph(2,3);
            }else if(a==0){
                break;
            }
            a = 1;
        }else{
            continue;
        }

        props = create_Properties(2);
        setIntegerProperty(0, 0, props);
        setIntegerProperty(0, 1, props);
        forum = create_Node(forumID,props);
        insertNode(forum,rec->forums);
    }
    fclose(fp);
}
/******************************************************************************/
void update_comment_replyOf_post(Graph *g,Graph *posts)
{
    FILE *fp;
    int postID,personID,creatorID,commentID,forumID,comments;
    char buffer[90];
    Record *creator,*r,*post,*person,*forum,*comment;
    Properties *props;
    Graph *commentg = create_Graph(2,3);

    if((fp = fopen("dataset/comment_hasCreator_person.csv","r")) == NULL){
        perror("File comment_hasCreator_person does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);

    // find repliers

    while(fscanf(fp,"%d|%d\n",&commentID,&personID) == 2){
        if((r = lookupNode(commentID, commentg)) == NULL){
            props = create_Properties(1);
            setIntegerProperty(personID,0,props);
            r = create_Node(commentID,props);
            insertNode(r,commentg);
        }
    }
    fclose(fp);

    if((fp = fopen("dataset/comment_replyOf_post.csv","r")) == NULL){
        perror("File comment_replyOf_post does not exist!\n");
        exit(1);
    }

    fscanf(fp, "%[^\n]",buffer);

    //collect the number of replies a person has on his posts from each other person

    while(fscanf(fp,"%d|%d",&commentID,&postID) == 2){
        comment = lookupNode(commentID, commentg);

        if(comment == NULL){
            continue;
        }

        post = lookupNode(postID,posts);

        if(post == NULL){
            continue;
        }

        forumID = getIntegerProperty(1,post->props);
        creatorID = getIntegerProperty(0,post->props);
        creator = lookupNode(creatorID,g);

        personID = getIntegerProperty(0, comment->props);
        person = lookupNode(personID,g);

        if(person!=NULL){
            if(person->likes == NULL){
                person->likes = create_Graph(2,3);
            }
        }else{
            continue;
        }


        if((creator = lookupNode(creatorID,person->likes)) == NULL){

            // insert the creator of post in the likes of the person

            props = create_Properties(1);
            setIntegerProperty(0,0,props);                    // total likes on creator
            creator = create_Node(creatorID,props);
            insertNode(creator,person->likes);

            // insert the forumID with the number of likes and replies on it

            creator->generic = create_Graph(2,3);            // forums
            props = create_Properties(2);
            setIntegerProperty(0,0,props);                    // likes
            setIntegerProperty(1,1,props);                    // replies
            forum = create_Node(forumID,props);
            insertNode(forum,creator->generic);
        }else{

            // increase replies on forum for this person

            if((forum = lookupNode(forumID, creator->generic)) == NULL){
                props = create_Properties(2);
                setIntegerProperty(0,0,props);
                setIntegerProperty(1,1,props);
                forum = create_Node(forumID,props);
                insertNode(forum,creator->generic);
            }else{
                comments = getIntegerProperty(1,forum->props);
                setIntegerProperty(comments+1,1,forum->props);
            }
        }

        // increase total replies on forum by person

        if(person->forums != NULL && (forum = lookupNode(forumID, person->forums)) != NULL) {
            comments = getIntegerProperty(1, forum->props);
            setIntegerProperty(comments+1, 1, forum->props);
        }
    }
    fclose(fp);

    destroyGraph3(&commentg, 1,0,-1,-1);
}
/******************************************************************************/
Graph* graph_tag(Graph *g)
{
    int id;
    FILE *file;
    char line[200];
    char name[100];
    Record *r;
    Properties *props;
    Graph *tags = create_Graph(2,3);

    if((file = fopen("dataset/tag.csv","r")) == NULL){
        perror("File tag does not exist!\n");
        exit(1);
    }
    fscanf(file, "%[^\n]", line);
    while(fscanf(file, "%d|%[^|]%[^\n]", &id, name, line) == 3){
        if(lookupNode(id, tags) == NULL){
            props = create_Properties(3);
            setStringProperty(name,2,props);
            r = create_Node(id,props);
            insertNode(r,tags);
        }
    }
    fclose(file);

    return tags;
}
/******************************************************************************/
