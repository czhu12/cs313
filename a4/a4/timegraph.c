#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "graph.h"


// DO NOT MAKE ANY CHANGES TO THIS FILE except if you want to 
// do multiple runs (trials) in which case you can change the 
// initialization of ntrials



static struct timeval start_ts, end_ts, diff_ts;

void startClock() {
    gettimeofday(&start_ts, 0);
}

void endClock() {
    gettimeofday(&end_ts, 0);
}

struct timeval timeminus(struct timeval a, struct timeval b)
{
    struct timeval res;
    res.tv_sec = a.tv_sec - b.tv_sec;
    if (a.tv_usec > b.tv_usec) {
	res.tv_usec = a.tv_usec - b.tv_usec;
    } else {
	res.tv_usec = 1000000 + a.tv_usec - b.tv_usec;
	res.tv_sec--;
    }
    return res;
}

long usecClock() {
    diff_ts = timeminus(end_ts, start_ts);
    return diff_ts.tv_usec + 1000000 * diff_ts.tv_sec;
}


// This is the convert function you are starting with, do no modify it


void base_convert(long G[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    G[j][i] = G[j][i] || G[i][j];
	}
    }
}


// This is prototype for the function you are going to improve. You will
// find the function to edit in convert.c
extern void convert(long G[N][N]);


// Initialize the directed graph with some random connections
void init (long G[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    G[i][j] = random() & 1;
	}
    }
}



// Used to compare the known (but slow) working solution results  
// against the "improved" one 

int identical(long G[N][N], long G2[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    if (G[i][j] != G2[i][j])
		return 0;
	}
    }
    return 1;
}


// Make a copy of the matrix
void copy(long G[N][N], long G2[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    G2[i][j] = G[i][j];
	}
    }
}

#define PARANOID 1

int main(int argc, char **argv) {
    long (*G)[N], (*G2)[N], (*G3)[N];
    G = calloc(N * N, sizeof(long));
    G2 = calloc(N * N, sizeof(long));
    G3 = calloc(N * N, sizeof(long));
    int trial, ntrials = 1;
    long thistime, besttime = 9999999999999999L;
    srandom(time(0));
    
    // Setup the matrix
    init(G);

    // Make a copy to compute the answer on
    if (PARANOID) copy(G, G2);
    
    // Compute the "right answer"
    if (PARANOID) base_convert(G2);

    // Make a copy to run the modified convert on 
    if (PARANOID && ntrials > 1) copy(G, G3);
    
    // Run a tial or trials 
    for (trial = 0; trial < ntrials; ++trial) {
	startClock();
	   convert(G);
	endClock();
	thistime = usecClock();
	
	// Check the answer
	if (PARANOID && !identical(G, G2))
	    printf("Got the wrong answer\n");
	    
        // If running more than 1 trial keep the best time
	if (thistime < besttime)
	    besttime = thistime;
	if (PARANOID && trial < ntrials - 1)
	    copy(G3, G);
    }
    printf("Convert took: %ld usec\n", besttime);
}
