// Girvan-Newman Algorithm for community discovery
// COMP2521 Assignment 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "GirvanNewman.h"
#include "Graph.h"
#include "FloydWarshall.h"

static Dendrogram DendrogramNew();
static void DoDendrogram(Graph g, Dendrogram dgm, int component[]);
static bool IsComponent(Graph g, Vertex src, Vertex dest);
/**
 * Generates  a Dendrogram for the given graph g using the Girvan-Newman
 * algorithm.
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram GirvanNewman(Graph g) {
	Dendrogram dgm = DendrogramNew();
	int numNodes = GraphNumVertices(g);

	// an initial array which contains all the vertex, 
	// since the initial graph has only one component
	int component[numNodes];
	for (int i = 0; i < numNodes; i++) {
		component[i] = 1;
	}

	// recursion function for constructing the Dendrogram
	DoDendrogram(g, dgm, component);

	return dgm;
}

/**
 * Frees all memory associated with the given Dendrogram  structure.  We
 * will call this function during testing, so you must implement it.
 */
void freeDendrogram(Dendrogram d) {
	if (d != NULL) {
		freeDendrogram(d->left);
		freeDendrogram(d->right);
		free(d);
	}
}

// a static function to create space for a new Dendrogram struct
static Dendrogram DendrogramNew () {
	Dendrogram dgm = malloc(sizeof(*dgm));
	dgm->left = NULL;
	dgm->right = NULL;
	return dgm;
}

// a static function to determine if there is any vertex links both src and dest
static bool IsComponent(Graph g, Vertex src, Vertex dest) {
	int numofvertex = GraphNumVertices(g);
	ShortestPaths sps = FloydWarshall(g);
	for (int i = 0; i < numofvertex; i++) {
		if ((sps.dist[src][i] != INFINITY) && (sps.dist[dest][i] != INFINITY)) {
			freeShortestPaths(sps);
			return true;
		} else if ((sps.dist[i][src] != INFINITY) && (sps.dist[i][dest] != INFINITY)) {
			freeShortestPaths(sps);
			return true;
		}
	}
	freeShortestPaths(sps);
	return false;
}

// recursion function to construct the Dendrogram
static void DoDendrogram(Graph g, Dendrogram dgm, int component[]) {
	EdgeValues e = edgeBetweennessCentrality(g);

	// find the current highest betweenness for the current component
	// assign the highest betweenness to max
	double max = -1;

	for (int i = 0; i < e.numNodes; i++) {
		for (int j = 0; j < e.numNodes; j++) {
			if((component[i] == 1) && (component[j] == 1)) {
				if (e.values[i][j] > max) {
					max = e.values[i][j];
				}
			}
		}
	}

	// create space for left child and right child
	Dendrogram left = DendrogramNew();
	Dendrogram right = DendrogramNew();
	dgm->left = left;
	dgm->right = right;

	// for the current Dendrogram, create two boolean to determine if 
	// its left and right childen will be a leaf
	bool LeftIsLeaf = false;
	bool RightIsLeaf = false;
	bool srcisleaf = false;
	for (int i = 0; i < e.numNodes; i++) {
		for (int j = 0; j < e.numNodes; j++) {
			// when the current edge's betweenness is belongs to the current component
			// and equal to the max, then remove it
			int src = i;
			int dest = j;
			if((component[src] == 1) && (component[dest] == 1)) {
				if (e.values[src][dest] == max) {
					GraphRemoveEdge(g, src, dest);

					// find the dist between the src and dest, if there is no path
					// from the src to dest or from dest to src, then go into the 
					// if statements, otherwise, keeps removing.

					ShortestPaths sps = FloydWarshall(g);
					if ((sps.dist[src][dest] == INFINITY) && (sps.dist[dest][src] == INFINITY) && !IsComponent(g, src, dest)) {
						
						// if there is no path between the src and dest, then it means that
						// they must be sperated components or leaves.
						// firstly determine if either one is a leaf or both are leaves
						if ((GraphOutIncident(g, src) == NULL) && (GraphInIncident(g, src) == NULL)) {
							left->vertex = src;
							LeftIsLeaf = true;
							srcisleaf = true;
						}

						if ((GraphOutIncident(g, dest) == NULL) && (GraphInIncident(g, dest) == NULL)) {
							if (LeftIsLeaf == false) {
								left->vertex = dest;
								LeftIsLeaf = true;
							} else {
								right->vertex = dest;
								RightIsLeaf = true;
							}
						}

						// when both children are inserted with a leaf, 
						// then back to the previous recursion
						if (RightIsLeaf) {
							return;
						}

						// when the left child is a leaf, then no more vertex can be inserted
						// so there is no need to go the left child's recursion
						if (LeftIsLeaf == false) {
							int leftComponent[e.numNodes];
							for (int i = 0; i < e.numNodes; i++) {
								leftComponent[i] = -1;
							}
							leftComponent[src] = 1;

							for (int v = 0; v < e.numNodes; v++) {
								if (component[v] == 1) {
									if ((sps.dist[v][src] != INFINITY) || sps.dist[src][v] != INFINITY) {
										leftComponent[v] = 1;
									}
								}
							}
							DoDendrogram(g, left, leftComponent);

							int rightComponent[e.numNodes];
							for (int i = 0; i < e.numNodes; i++) {
								rightComponent[i] = -1;
							}
							rightComponent[dest] = 1;
							
							for (int v = 0; v < e.numNodes; v++) {
								if (component[v] == 1) {
									if ((sps.dist[v][dest] != INFINITY) || sps.dist[dest][v] != INFINITY) {
										rightComponent[v] = 1;
									}
								}
							}
							
							DoDendrogram(g, right, rightComponent);
							freeShortestPaths(sps);


						} else {
							int rightComponent[e.numNodes];
							if (srcisleaf) {
								for (int i = 0; i < e.numNodes; i++) {
									rightComponent[i] = -1;
								}
								rightComponent[dest] = 1;
								
								for (int v = 0; v < e.numNodes; v++) {
									if (component[v] == 1) {
										if ((sps.dist[v][dest] != INFINITY) || sps.dist[dest][v] != INFINITY) {
											rightComponent[v] = 1;
										}
									}
								}
								
								DoDendrogram(g, right, rightComponent);
								freeShortestPaths(sps);
							} else {
								for (int i = 0; i < e.numNodes; i++) {
									rightComponent[i] = -1;
								}
								rightComponent[src] = 1;
								
								for (int v = 0; v < e.numNodes; v++) {
									if (component[v] == 1) {
										if ((sps.dist[v][src] != INFINITY) || sps.dist[src][v] != INFINITY) {
											rightComponent[v] = 1;
										}
									}
								}
								
								DoDendrogram(g, right, rightComponent);
								freeShortestPaths(sps);
							}
						}

						// the code would not reach here unless the left child is a leaf
						// and the right child is a component, so keeps doing recursion for
						// the right child
					}					
				}
			}
		}
	}
	freeEdgeValues(e);
}
