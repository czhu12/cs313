#include <stdio.h>
#include "graph.h"

// This is the function you are to improve the performance of 
// In this file you are to have a copy of every version of 
// convert in your log file. So that you don't have to modify
// timegraph.c as you write a new version of convert give it a
// unique, meaningful name (record the name in the log  
// you are keeping) and then change the first part of the 
// define below to be that name. For example if you 
// add the convert function named convert_try1  then you 
// would change the #define to 
// #define convert_try1  convert.

#define  convert_orig convert


void convert_orig(long G[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
	for (j = 0; j < N; ++j) {
	    G[j][i] = G[j][i] || G[i][j];
	}
    }
}
