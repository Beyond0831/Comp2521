// Floyd Warshall ADT interface
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "FloydWarshall.h"
#include "Graph.h"

/**
 * Finds all shortest paths between all pairs of nodes.
 * 
 * The  function  returns  a 'ShortestPaths' structure with the required
 * information:
 * - the number of vertices in the graph
 * - distance matrix
 * - matrix of intermediates (see description above)
 */
ShortestPaths FloydWarshall(Graph g) {
	ShortestPaths sps;
	sps.numNodes = GraphNumVertices(g);

	sps.dist = calloc(sps.numNodes, sizeof(int *));
	for (int v = 0; v < sps.numNodes; v++) {
		sps.dist[v] = calloc(sps.numNodes, sizeof(int));
	}

	for (int i = 0; i < sps.numNodes; i++) {
		for (int j = 0; j < sps.numNodes; j++) {
			sps.dist[i][j] = INFINITY;
		}
	}

	// line 24~33 initialize the matrix dist

	sps.next = calloc(sps.numNodes, sizeof(int *));
	for (int v = 0; v < sps.numNodes; v++) {
		sps.next[v] = calloc(sps.numNodes, sizeof(int));
	}

	for (int i = 0; i < sps.numNodes; i++) {
		for (int j = 0; j < sps.numNodes; j++) {
			sps.next[i][j] = -1;
		}
	}
	// line 37~46 initialze the matrix next

	for (int v = 0; v < sps.numNodes; v++) {
		sps.dist[v][v] = 0;
	}

	for (int i = 0; i < sps.numNodes; i++) {
		AdjList CurrentVertice = GraphOutIncident(g, i);
		while (CurrentVertice != NULL) {
			int dest = CurrentVertice->v;
			sps.dist[i][dest] = CurrentVertice->weight;
			sps.next[i][dest] = dest;
			CurrentVertice = CurrentVertice->next;
		}
	}


	for (int k = 0; k < sps.numNodes; k++) {
		for (int i = 0; i < sps.numNodes; i++) {
			for (int j = 0; j < sps.numNodes; j++) {
				if ((sps.dist[i][j] > sps.dist[i][k] + sps.dist[k][j]) &&
				   (sps.dist[i][k] != INFINITY) && (sps.dist[k][j] != INFINITY)) {
					sps.dist[i][j] = sps.dist[i][k] + sps.dist[k][j];
					sps.next[i][j] = sps.next[i][k];
				}
			}
		}
	}


	return sps;
}

/**
 * This  function  is  for  you to print out the ShortestPaths structure
 * while you are debugging/testing your implementation. 
 * 
 * We will not call this function during testing, so you may  print  out
 * the  given  ShortestPaths  structure in whatever format you want. You
 * may choose not to implement this function.
 */
void showShortestPaths(ShortestPaths sps) {

}

/**
 * Frees  all  memory associated with the given ShortestPaths structure.
 * We will call this function during testing, so you must implement it.
 */
void freeShortestPaths(ShortestPaths sps) {
	for (int v = 0; v < sps.numNodes; v++) {
		free(sps.dist[v]);
		free(sps.next[v]);
	}
	free(sps.dist);
	free(sps.next);
}

