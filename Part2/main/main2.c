#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../graph/graph.h"
#include "main_functions.h"
#include "../metrics/metrics.h"
#include "../queries/queries.h"

#define CHECKINT(text, actual, expected)                         \
  if (actual != expected) {                                   \
    printf("%-30s: Failed | actual = %3d, expected = %3d\n",  \
           text,                                              \
           actual,                                            \
           expected);                                         \
  } else {                                                    \
    printf("%-30s: Success\n", text);                     \
  }

#define CHECKDOUBLE(text, actual, expected)                         \
if ( ABS(actual,expected) > 0.01 ) {                                   \
  printf("%-30s: Failed | actual = %3f, expected = %3f\n",  \
         text,                                              \
         actual,                                            \
         expected);                                         \
} else {                                                    \
  printf("%-30s: Success\n", text);                     \
}

#define ABS(value1, value2) (value1 - value2 < 0 ) ? value2 - value1 : value1 - value2



void testBetweennessCentrality(int bucketsNumber, int bucketSize);
void testClosenessCentrality(int bucketsNumber, int bucketSize);
void testTidalTrust(int bucketsNumber, int bucketSize);
void stalkersGraphRunMetrics(Graph* gStalkers);

int main(int argc , char *argv[])
{
    //firstly execute a test for centrality metrics on small graphs

    int m = 2;
    int c = 3;

    testBetweennessCentrality(m, c);

    testClosenessCentrality(m, c);

    //small example for trust graph estimation
    testTidalTrust(m, c);
    
    printf("\n");
    
	/*create graph from file*/
    
    int bucketsNumber = 10;
    int bucketSize = 5;
    Graph* g;
    g = graph_person(bucketsNumber,bucketSize);	
	update_person_knows_person(g);
	

    //graph metrics calls

    //plot the graph degree distribution
    degreeDistribution(g);

    int diam = diameter(g);
    CHECKINT("Graph diameter", diam, 14);

    double avgPthLgth = averagePathLength(g);
    CHECKDOUBLE("Graph average path length", avgPthLgth, 5.0322);

    int ccNumber = numberOfCCs(g);
    CHECKINT("Graph number of components ", ccNumber, 1);

    int maximumCC = maxCC(g);
    CHECKINT("Graph maximum connected component ", maximumCC, 111);

    double dense = density(g);
    CHECKDOUBLE("Graph density ", dense, 0.073);
    
    int closenessIds[5] = {1734, 38, 8899, 3501, 75};
    float closenessIdsRes[5] = {0.3430,0.3439,0.3147,0.2667,0.1594};
    int i;
    for (i = 0; i < 5; ++i) {
        int nodeID = closenessIds[i];
        Record* node = lookupNode(nodeID, g);
        double closCentrty = closenessCentrality(node, g);
        CHECKDOUBLE("Graph closeness centrality ", closCentrty, closenessIdsRes[i]);
    }

    int betweennessIds[5] = { 1734, 38, 8899, 9900, 75};
    float betweennessIdsRes[5] = { 0.306, 0.053, 0.018, 0.005, 0.000};

    for (i = 0; i < 5; ++i) {
        int nodeID = betweennessIds[i];
        Record* node = lookupNode(nodeID, g);
        double betwCentrty = betweennessCentrality(node, g);
        CHECKDOUBLE("Graph betweenness centrality ", betwCentrty, betweennessIdsRes[i]);
    }
    
    printf("\n");
  
    
    //graph queries calls
    // Query 1 //       
    Matches* match;
    Record *dateNode = lookupNode(3755, g);
    int commonInterests = 1, ageDiff = 30, acquaintanceHops = 3, matchesNum = 1;
    match = matchSuggestion(dateNode, commonInterests, acquaintanceHops, ageDiff, matchesNum, g);

    //match result : 7107 - work_at_organization: 1650
    //get first pair's person ids
    int id1 = match[0].id;

    CHECKINT("Query 1: Date result 1st id", id1, 7107);
    
    free(match);
    
    printf("\n");
      
	int k;
    // Query 2 //
    //estimate stalkers graph with closeness centrality     
    Graph* stalkersGraphCloseCentr;
    
    int stalkersNum = 7, likesNumber = 1, centralityMode = 1;
    Stalkers* stalkersCloseCentr = malloc(sizeof(Stalkers)*stalkersNum);
    stalkersGraphCloseCentr = getTopStalkers(stalkersNum, likesNumber, centralityMode, g, stalkersCloseCentr);

    int stalkersResultsIds[] = {347, 495, 7768, 8354, 8403, 8899, 9633};
    int stalkerResultSize = 7;
    int stalkerID;
    int counter = 0;
    
    for (i = 0; i < stalkersNum; ++i) {
        stalkerID = stalkersCloseCentr[i].id;
        if (stalkerID != -1) {
            ++counter;
        }
        for (k = 0; k < stalkerResultSize; ++k) {
            if (stalkersResultsIds[k] == stalkerID) {
                break;
            }
        }
        if (k == stalkerResultSize) {
            printf("You wrongly labeled person with id %d as Stalker\n", stalkerID);
        }
    }
    CHECKINT("Query 2: Stalker closeness results size", stalkerResultSize, counter);

    //run metrics on stalker-graph
    stalkersGraphRunMetrics(stalkersGraphCloseCentr);

    free(stalkersCloseCentr);
    destroyGraph3(&stalkersGraphCloseCentr, 0,1,-1,-1);

	 //estimate stalkers graph with betweenness centrality 
    Graph* stalkersGraphBetwCentr;
    Stalkers* stalkersBetwCentr = malloc(sizeof(Stalkers)*stalkersNum);
    centralityMode = 2;
    counter = 0;
    stalkersGraphBetwCentr = getTopStalkers(stalkersNum, likesNumber, centralityMode, g, stalkersBetwCentr);

    for (i = 0; i < stalkersNum; ++i) {
        stalkerID = stalkersBetwCentr[i].id;
        if (stalkerID != -1) {
            ++counter;
        }
        for (k = 0; k < stalkerResultSize; ++k) {
            if (stalkersResultsIds[k] == stalkerID) {
                break;
            }
        }
        if (k == stalkerResultSize) {
            printf("You wrongly labeled person with id %d as Stalker\n", stalkerID);
        }
    }

    CHECKINT("Query 2: Stalker betweenness results size", stalkerResultSize, counter);


    //run metrics on stalker-graph
    stalkersGraphRunMetrics(stalkersGraphBetwCentr); 
    
    free(stalkersBetwCentr);
    destroyGraph3(&stalkersGraphBetwCentr, 0,1,-1,-1);
    
	printf("\n"); 
  
    int trendsNum = 4;
    
    char** womenTrends = malloc(trendsNum*sizeof(char*));
	char** menTrends = malloc(trendsNum*sizeof(char*));	
	
    findTrends(trendsNum, g, womenTrends, menTrends);

    printf("Top Women - Men Trends");
    int j;
    char* menTrendsResults[4] = {"Sun_Yat-sen", "Constantine_the_Great",
        "Sigmund_Freud", "Hussein_of_Jordan"}; //IDS: {417,11622,468,1398}
    char* womenTrendsResults[4] = {"Adolf_Hitler", "Chiang_Kai-shek",NULL,NULL}; //IDS: {138,416,null,null}

    int counterW = 0, counterM = 0;

    for (j = 0; j < 4; ++j) {
        if (menTrends[j] != NULL) {
            ++counterM;

            for (k = 0; k < 4; ++k) {

                if (strcmp(menTrends[j], menTrendsResults[k]) == 0) {
                    break;
                }
            }
            if (k == 4) {
                printf("You wrongly labeled tag with %s as top trend\n", menTrends[j]);
            }
        }
    }
    CHECKINT("Query 3: Mens Trends result size", 4, counterM);	

    for (j = 0; j < 4; ++j) {
        if (womenTrends[j] != NULL) {
            ++counterW;

            for (k = 0; k < 2; ++k) {
				
                if (strcmp(womenTrends[j], womenTrendsResults[k]) == 0) {
                    break;
                }
            }
            if (k == 2) {
                printf("You wrongly labeled tag with %s as top trend\n", womenTrends[j]);
            }
        }
    }

    CHECKINT("Query 3: Women Trends result size", 2, counterW);	      

    
	for(i=0;i<trendsNum; i++){
		if(menTrends[i] != NULL){
			free(menTrends[i]);
		}
		if(womenTrends[i] != NULL){
			free(womenTrends[i]);
		}
	}

	free(menTrends);
	free(womenTrends);
    
    printf("\n"); 
	

    Graph* trustGraph;
    int forumID = 34680;
	
    trustGraph = buildTrustGraph(forumID, g);

    int trustANodeId = 30;
    int trustBNodeId = 9805;
    int trustCNodeId = 9700;
    Record *ta = lookupNode(trustANodeId, trustGraph);
    Record *tb = lookupNode(trustBNodeId, trustGraph);
    Record *tc = lookupNode(trustCNodeId, trustGraph);
    double trustAB;
    trustAB = estimateTrust(ta, tb, trustGraph);
    printf("Trust between nodes (%d,%d) is %f\n", trustANodeId, trustBNodeId, trustAB);
    CHECKDOUBLE("Trust: (30,9805) ", trustAB, 0.134);

    double trustAC;
    trustAC = estimateTrust(ta, tc, trustGraph);
    printf("Trust between nodes (%d,%d) is %f\n", trustANodeId, trustCNodeId, trustAC);
    CHECKDOUBLE("Trust: (30,9700) ", trustAC, 0.15);

	//destroyGraph3(&trustGraph, 0,2,-1,-1);

	destroyGraph(&g);
	
	return 1;
}

void testBetweennessCentrality(int bucketsNumber, int bucketSize) {
    //create small graph for testing betweenness Centrality
    Graph* gBetw = create_Graph(bucketsNumber, bucketSize);

    Record* n1Betw = create_Node(1, NULL);
    Record* n2Betw = create_Node(2, NULL);
    Record* n3Betw = create_Node(3, NULL);
    Record* n4Betw = create_Node(4, NULL);
    Record* n5Betw = create_Node(5, NULL);

    insertNode(n1Betw, gBetw);
    insertNode(n2Betw, gBetw);
    insertNode(n3Betw, gBetw);
    insertNode(n4Betw, gBetw);
    insertNode(n5Betw, gBetw);

    /* Create edges and set properties */
    Edge* e1Betw = create_Edge(1, 2, NULL);
    Edge* e2Betw = create_Edge(2, 1, NULL);
    Edge* e3Betw = create_Edge(2, 3, NULL);
    Edge* e4Betw = create_Edge(2, 4, NULL);
    Edge* e5Betw = create_Edge(3, 2, NULL);
    Edge* e6Betw = create_Edge(3, 5, NULL);
    Edge* e7Betw = create_Edge(4, 2, NULL);
    Edge* e8Betw = create_Edge(4, 5, NULL);
    Edge* e9Betw = create_Edge(5, 3, NULL);
    Edge* e10Betw = create_Edge(5, 4, NULL);

    /* Insert edges in graph */
    insertEdge(1, e1Betw, gBetw);
    insertEdge(2, e2Betw, gBetw);
    insertEdge(2, e3Betw, gBetw);
    insertEdge(2, e4Betw, gBetw);
    insertEdge(3, e5Betw, gBetw);
    insertEdge(3, e6Betw, gBetw);
    insertEdge(4, e7Betw, gBetw);
    insertEdge(4, e8Betw, gBetw);
    insertEdge(5, e9Betw, gBetw);
    insertEdge(5, e10Betw, gBetw);

    double betwCentrty1 = betweennessCentrality(n1Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:1 ", betwCentrty1, 0.0 / 6);

    double betwCentrty2 = betweennessCentrality(n2Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:2 ", betwCentrty2, 3.5 / 6);

    double betwCentrty3 = betweennessCentrality(n3Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:3 ", betwCentrty3, 1.0 / 6);

    double betwCentrty4 = betweennessCentrality(n4Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:4 ", betwCentrty4, 1.0 / 6);

    double betwCentrty5 = betweennessCentrality(n5Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:5 ", betwCentrty5, 0.5 / 6);
    
    destroyGraph(&gBetw);
}

void testClosenessCentrality(int bucketsNumber, int bucketSize) {
    //create small graph for testing betweenness Centrality
    Graph* gClos = create_Graph(bucketsNumber, bucketSize);

    Record* n1Clos = create_Node(1, NULL);
    Record* n2Clos = create_Node(2, NULL);
    Record* n3Clos = create_Node(3, NULL);
    Record* n4Clos = create_Node(4, NULL);
    Record* n5Clos = create_Node(5, NULL);
    Record* n6Clos = create_Node(6, NULL);
    Record* n7Clos = create_Node(7, NULL);

    insertNode(n1Clos, gClos);
    insertNode(n2Clos, gClos);
    insertNode(n3Clos, gClos);
    insertNode(n4Clos, gClos);
    insertNode(n5Clos, gClos);
    insertNode(n6Clos, gClos);
    insertNode(n7Clos, gClos);

    /* Create edges and set properties */
    Edge* e1Clos = create_Edge(1, 2, NULL);
    Edge* e2Clos = create_Edge(1, 3, NULL);
    Edge* e3Clos = create_Edge(2, 1, NULL);
    Edge* e4Clos = create_Edge(2, 3, NULL);
    Edge* e5Clos = create_Edge(3, 1, NULL);
    Edge* e6Clos = create_Edge(3, 2, NULL);
    Edge* e7Clos = create_Edge(3, 4, NULL);
    Edge* e8Clos = create_Edge(4, 3, NULL);
    Edge* e9Clos = create_Edge(4, 5, NULL);
    Edge* e10Clos = create_Edge(5, 4, NULL);
    Edge* e11Clos = create_Edge(5, 6, NULL);
    Edge* e12Clos = create_Edge(5, 7, NULL);
    Edge* e13Clos = create_Edge(6, 5, NULL);
    Edge* e14Clos = create_Edge(6, 7, NULL);
    Edge* e15Clos = create_Edge(7, 5, NULL);
    Edge* e16Clos = create_Edge(7, 6, NULL);


    /* Insert edges in graph */
    insertEdge(1, e1Clos, gClos);
    insertEdge(1, e2Clos, gClos);
    insertEdge(2, e3Clos, gClos);
    insertEdge(2, e4Clos, gClos);
    insertEdge(3, e5Clos, gClos);
    insertEdge(3, e6Clos, gClos);
    insertEdge(3, e7Clos, gClos);
    insertEdge(4, e8Clos, gClos);
    insertEdge(4, e9Clos, gClos);
    insertEdge(5, e10Clos, gClos);
    
    insertEdge(5, e11Clos, gClos);
    insertEdge(5, e12Clos, gClos);
    insertEdge(6, e13Clos, gClos);
    insertEdge(6, e14Clos, gClos);
    insertEdge(7, e15Clos, gClos);
    insertEdge(7, e16Clos, gClos);


    double closCentrty1 = closenessCentrality(n1Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:1 ", closCentrty1, 3.33 / 6.0);

    double closCentrty2 = closenessCentrality(n2Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:2 ", closCentrty2, 3.33 / 6.0);

    double closCentrty3 = closenessCentrality(n3Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:3 ", closCentrty3, 4.16 / 6.0);

    double closCentrty4 = closenessCentrality(n4Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:4 ", closCentrty4, 4.0 / 6.0);

    double closCentrty5 = closenessCentrality(n5Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:5 ", closCentrty5, 4.16 / 6.0);

    double closCentrty6 = closenessCentrality(n6Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:6 ", closCentrty6, 3.33 / 6.0);

    double closCentrty7 = closenessCentrality(n7Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:7 ", closCentrty7, 3.33 / 6.0);
    
    destroyGraph(&gClos);
}

void testTidalTrust(int bucketsNumber, int bucketSize) {
    //create small graph for testing tidal's trust algorithm result
    Graph* gtt = create_Graph(bucketsNumber, bucketSize);

    Record* n1tt = create_Node(1, NULL);
    Record* n2tt = create_Node(2, NULL);
    Record* n3tt = create_Node(3, NULL);
    Record* n4tt = create_Node(4, NULL);
    Record* n5tt = create_Node(5, NULL);
    Record* n6tt = create_Node(6, NULL);
    Record* n7tt = create_Node(7, NULL);
    Record* n8tt = create_Node(8, NULL);
    Record* n9tt = create_Node(9, NULL);
    Record* n10tt = create_Node(10, NULL);
    Record* n11tt = create_Node(11, NULL);

    insertNode(n1tt, gtt);
    insertNode(n2tt, gtt);
    insertNode(n3tt, gtt);
    insertNode(n4tt, gtt);
    insertNode(n5tt, gtt);
    insertNode(n6tt, gtt);
    insertNode(n7tt, gtt);
    insertNode(n8tt, gtt);
    insertNode(n9tt, gtt);
    insertNode(n10tt, gtt);
    insertNode(n11tt, gtt);


    Edge* e1tt = setWeightedEdgeProperties(1, 2, "trust", 1.0);
    Edge* e2tt = setWeightedEdgeProperties(1, 5, "trust",1.0);
    Edge* e3tt = setWeightedEdgeProperties(2, 3, "trust",0.9);
    Edge* e4tt = setWeightedEdgeProperties(2, 4, "trust",0.9);
    Edge* e5tt = setWeightedEdgeProperties(3, 6, "trust",0.8);
    Edge* e6tt = setWeightedEdgeProperties(4, 6, "trust",0.3);
    Edge* e7tt = setWeightedEdgeProperties(4, 7, "trust",0.9);
    Edge* e8tt = setWeightedEdgeProperties(5, 10, "trust",0.9);
    Edge* e9tt = setWeightedEdgeProperties(6, 9, "trust",1.0);
    Edge* e10tt = setWeightedEdgeProperties(7, 8, "trust",1.0);
    Edge* e11tt = setWeightedEdgeProperties(8, 9, "trust",1.0);
    Edge* e12tt = setWeightedEdgeProperties(9, 11, "trust",1.0);
    Edge* e13tt = setWeightedEdgeProperties(10, 11, "trust",0.4);

    /* Insert edges in graph */
    insertEdge(1, e1tt, gtt);
    insertEdge(1, e2tt, gtt);
    insertEdge(2, e3tt, gtt);
    insertEdge(2, e4tt, gtt);
    insertEdge(3, e5tt, gtt);
    insertEdge(4, e6tt, gtt);
    insertEdge(4, e7tt, gtt);
    insertEdge(5, e8tt, gtt);
    insertEdge(6, e9tt, gtt);
    insertEdge(7, e10tt, gtt);
    insertEdge(8, e11tt, gtt);
    insertEdge(9, e12tt, gtt);
    insertEdge(10, e13tt, gtt);

    Record *att = lookupNode(1, gtt);

    Record *btt = lookupNode(11, gtt);
    //Estimate trust(1,11)
    double trust1_11 = estimateTrust(att, btt, gtt);
    CHECKDOUBLE("Graph estimate trust (1,11)", trust1_11, 0.36);

    //Estimate trust(1,9)
    Record *ctt = lookupNode(9, gtt);
    double trust1_9 = estimateTrust(att, ctt, gtt);
    CHECKDOUBLE("Graph estimate trust (1,9)", trust1_9, 0.72);

	destroyGraph3(&gtt, 0, PERSON_REL_PROPERTIES_NUM+1,-1,-1);
}

void stalkersGraphRunMetrics(Graph* gStalkers) {

    printf("Stalkers graph executing metrics:\n");
    degreeDistribution(gStalkers);

    int diam = diameter(gStalkers);
	printf("diameter = %d\n", diam);
    double avgPthLgth = averagePathLength(gStalkers);
	printf("avgPathLength = %lf\n", avgPthLgth);
    int ccNumber = numberOfCCs(gStalkers);
	printf("ccNumber = %d\n", ccNumber);
    int maximumCC = maxCC(gStalkers);
	printf("maximumCC = %d\n", maximumCC);
    double dense = density(gStalkers);
	printf("dense = %lf\n", dense);


}
