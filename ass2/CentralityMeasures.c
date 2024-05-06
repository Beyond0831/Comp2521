// Centrality Measures ADT interface
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "FloydWarshall.h"
#include "Graph.h"

/**
 * Finds  the  edge  betweenness  centrality  for each edge in the given
 * graph and returns the results in a  EdgeValues  structure.  The  edge
 * betweenness centrality of a non-existant edge should be set to -1.0.
 */
EdgeValues edgeBetweennessCentrality(Graph g) {
	ShortestPaths sps = FloydWarshall(g);

	//line 20 ~ 31 initialize the value matrix
	EdgeValues e = {.numNodes = sps.numNodes};
	e.values = calloc(e.numNodes, sizeof(double *));
	for (int v = 0; v < e.numNodes; v++) {
		e.values[v] = calloc(e.numNodes, sizeof(double));
	}

	for (int i = 0; i < e.numNodes; i++) {
		for (int j = 0; j < e.numNodes; j++) {
			e.values[i][j] = -1.0;
		}
	}

	// line 33 ~ 41: if the edge exists, its value would be set to 0
	for (int i = 0; i < e.numNodes; i++) {
		AdjList CurrentVertice = GraphOutIncident(g, i);
		while (CurrentVertice != NULL) {
			int dest = CurrentVertice->v;
			e.values[i][dest] = 0.0;
			CurrentVertice = CurrentVertice->next;
		}
	}



	for (int i = 0; i < e.numNodes; i++) {
		for (int j = 0; j < e.numNodes; j++) {
			if (sps.dist[i][j] != INFINITY) {
				int curr = i, dest = j;
				while (curr != dest)  {
					int next = sps.next[curr][dest];
					e.values[curr][next]++;
					curr = next;
				}
			}
		}
	}

	
	freeShortestPaths(sps);
	return e;
}

/**
 * Prints  the  values in the given EdgeValues structure to stdout. This
 * function is purely for debugging purposes and will NOT be marked.
 */
void showEdgeValues(EdgeValues evs) {

}

/**
 * Frees all memory associated with the given EdgeValues  structure.  We
 * will call this function during testing, so you must implement it.
 */
void freeEdgeValues(EdgeValues evs) {
	for (int v = 0; v < evs.numNodes; v++) {
		free(evs.values[v]);
	}
	free(evs.values);
}


