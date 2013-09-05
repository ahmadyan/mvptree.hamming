
#include <iostream>
#include <vector>
#include "object.h"
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

extern "C"{
	#include "mvptree.h"
}
#define MVP_BRANCHFACTOR 2
#define MVP_PATHLENGTH   5
#define MVP_LEAFCAP     25

void testmvpfree(void *ptr){
	free(ptr);
}

static unsigned long long nbcalcs = 0;

float hamming_distance(MVPDP *pointA, MVPDP *pointB){
	if (!pointA || !pointB) {
		return -1.0f;
	}
	if (!pointA->data || !pointB->data) {
		return -1.0f;
	}
	return ((Object*)(pointA->data))->distance( (Object*)(pointB->data));
}

MVPDP* generate_point(const unsigned int dp_length){
	cout << dp_length << endl ;
	Object* o = new Object(dp_length);
	o->randomize();
	MVPDP* node = new MVPDP();
	node->datalen = dp_length;
	node->data = o;
	char scratch[32];
	snprintf(scratch, 32, "point%llu", o->getID());
	node->id = strdup(scratch);
	cout << node->id << "   " << o->toString() << endl ;
	return node;
}

/* generate random data points for a tree */
MVPDP** generate_uniform_points(const unsigned int nbpoints, const unsigned int dp_length){
	/* generate nbpoints uniformly distributed points all of dp_length BYTEARRAY */
	MVPDP **pointlist = (MVPDP**)malloc(nbpoints*sizeof(MVPDP*));
	if (pointlist == NULL){
		return NULL;
	}
	unsigned int i;
	for (i=0;i < nbpoints;i++){
		pointlist[i] = generate_point(dp_length);
		if (!pointlist[i]) return NULL;
	}

	return pointlist;
}

int main(int argc, char **argv){
	vector<Object*> v;
	 unsigned int nbpoints = 100;
	 float radius = 5.0f;
	 unsigned int knearest = nbpoints;
	 unsigned int dplength = 10;
	CmpFunc distance_func = hamming_distance;
	srand(98293928);

	/* generate points */
	MVPDP  **pointlist = generate_uniform_points(nbpoints, dplength);
	assert(pointlist);

	/* create tree */
	MVPTree *tree = mvptree_alloc(NULL, distance_func, MVP_BRANCHFACTOR, MVP_PATHLENGTH, MVP_LEAFCAP);
	assert(tree);

	printf("Add list of %d points to tree.\n", nbpoints);
	/* add points to tree */
	MVPError err = mvptree_add(tree, pointlist, nbpoints);
	assert(err == MVP_SUCCESS);



	/* free the point lists but not the points themselves */
	/* the tree takes ownership of the points!!! */
	free(pointlist);


	//printf("Write tree out to file, %s.\n", testfile);
	/* write out the tree */
	//err = mvptree_write(tree, testfile, 00755);
	//assert(err == MVP_SUCCESS);


	//mvptree_clear(tree, free);
	//free(tree);


	//printf("Read tree from %s.\n", testfile);
	/* read it back in again */
	//tree = mvptree_read(testfile, distance_func, MVP_BRANCHFACTOR, MVP_PATHLENGTH, MVP_LEAFCAP, &err);
	//assert(tree);


	/* add some points to a tree one at a time */
	//MVPDP *savedpoint = NULL;
	int count = 0, total = nbpoints/10;
	do {
		MVPDP *pnt = generate_point(dplength);

		printf("Add point, %s to tree.\n", pnt->id);
		err = mvptree_add(tree, &pnt, 1);
		assert(err == MVP_SUCCESS);
	} while (++count < total);



	/* retrieve the cluster center
	nbcalcs = 0;
	unsigned int nbresults;
	MVPDP **results = mvptree_retrieve(tree, cluster_center , knearest, radius, &nbresults, &err);
	assert(results);
	assert(err == MVP_SUCCESS);

	unsigned int i;
	for (i = 0;i < nbresults;i++){
		fprintf(stdout,"  FOUND --> (%d) %s\n", i, results[i]->id);
	}
	free(results);
	*/

	MVPDP* savedpoint = generate_point(dplength);
	radius=2;
	printf("\nRetrieve point, %s.  : ", savedpoint->id);
	string s  = ((Object*)(savedpoint->data))->toString() ;
	cout << s<< endl ;
	nbcalcs = 0;
	unsigned int nbresults;
	/* retrieve the first saved point */
	nbcalcs = 0;
	MVPDP **results = mvptree_retrieve(tree, savedpoint, knearest, radius, &nbresults, &err);
	assert(results);
	assert(err == MVP_SUCCESS);

	for (int i=0;i<nbresults;i++){
		fprintf(stdout,"  FOUND --> (%d) %s  : ", i, results[i]->id);
		cout << ((Object*)(results[i]->data))->toString() << endl ;
	}

	free(results);
	mvptree_clear(tree, free);
	free(tree);

	//dp_free(cluster_center, free);
	dp_free(savedpoint, free);

	printf("Done.\n");
	return 0;
}
