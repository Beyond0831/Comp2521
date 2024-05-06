
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "AVLTree.h"

typedef struct node *Node;
struct node {
    Record rec;
    Node   left;
    Node   right;
    int    height;
};

struct tree {
    Node    root;
    int     (*compare)(Record, Record);
};

////////////////////////////////////////////////////////////////////////
// Auxiliary functions

static void doTreeFree(Node n, bool freeRecords);
static Node newNode(Record rec);
static Record doTreeSearch(Tree t, Node n, Record rec);

////////////////////////////////////////////////////////////////////////

static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->rec = rec;
    n->left = NULL;
    n->right = NULL;
    n->height = 0;
    return n;
}

////////////////////////////////////////////////////////////////////////

Tree TreeNew(int (*compare)(Record, Record)) {
    Tree t = malloc(sizeof(*t));
    if (t == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    t->root = NULL;
    t->compare = compare;
    return t;
}

////////////////////////////////////////////////////////////////////////

void TreeFree(Tree t, bool freeRecords) {
    doTreeFree(t->root, freeRecords);
    free(t);
}

static void doTreeFree(Node n, bool freeRecords) {
    if (n != NULL) {
        doTreeFree(n->left, freeRecords);
        doTreeFree(n->right, freeRecords);
        if (freeRecords) {
            RecordFree(n->rec);
        }
        free(n);
    }
}

////////////////////////////////////////////////////////////////////////

Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

static Record doTreeSearch(Tree t, Node n, Record rec) {
    if (n == NULL) {
        return NULL;
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        return doTreeSearch(t, n->left, rec);
    } else if (cmp > 0) {
        return doTreeSearch(t, n->right, rec);
    } else {
        return n->rec;
    }
}


////////////////////////////////////////////////////////////////////////
/* IMPORTANT: 
   Do NOT modify the code above this line. 
   You must not modify the 'node' and 'tree' structures defined above.
   You must not modify the functions defined above.
*/
////////////////////////////////////////////////////////////////////////
// Auxiliary functions
static Node doTreeInsert(Tree t, Node n, Record rec, bool *res);
static int height(Node n);              // a function to get the height of a node in the tree
static Node RotateLeft(Node n);         // move right child to root and rearrange links to retain order
static Node RotateRight(Node n);        // move left child to root and rearrange links to retain order

static void doTreeSearchBetween(Tree t, Node n, Record lower,
    Record upper, List l);

static void doTreeNext(Tree t, Node n, Record r, Record *next);
////////////////////////////////////////////////////////////////////////

bool TreeInsert(Tree t, Record rec) {
    bool res = false; // assume that the insertion is done first, 
                      // and becomes true if the insertion succeed.
    t->root = doTreeInsert(t, t->root, rec, &res);
    return res;
}

static Node doTreeInsert(Tree t, Node n, Record rec, bool *res) {
    if (n == NULL) {   //base case
        *res = true;
        return newNode(rec);
    }

    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        n->left = doTreeInsert(t, n->left, rec, res);
    } else if (cmp > 0) {
        n->right = doTreeInsert(t, n->right, rec, res);
    } else {           //when the record is already in the tree
        *res = false;
        return n;
    }

    int Lheight = height(n->left);
    int Rheight = height(n->right);
    if (*res) {
        if (height(n->left) >= height(n->right)) {
            n->height = height(n->left) + 1;
        } else {
            n->height = height(n->right) + 1;
        }
    }
    bool rotate = false;
    if (Lheight - Rheight > 1) {
        rotate = true;
        int cmpleft = t->compare(rec, n->left->rec);
        if (cmpleft < 0) {
            n = RotateRight(n);
        } else {      // cmpleft > 0 
            n->left = RotateLeft(n->left);
            n = RotateRight(n);
        }
    } else if (Rheight - Lheight > 1) {
        rotate = true;
        int cmpright = t->compare(rec, n->right->rec);
        if (cmpright > 0) {
            n = RotateLeft(n);
        } else {      // cmpright < 0 
            n->right = RotateRight(n->right);
            n = RotateLeft(n);
        }   
    }

    if (rotate) {
        n->height = 1;
        n->left->height = 0;
        n->right->height = 0;
    }
    return n;

}

static int height(Node n) {
    if (n == NULL) {
        return -1;
    }
    return n->height;
}

static Node RotateLeft(Node n) {
    Node node_right = n->right;
    n->right = node_right->left;
    node_right->left = n;
    return node_right;
}

static Node RotateRight(Node n) {
    Node node_left = n->left;
    n->left = node_left->right;
    node_left->right = n;
    return node_left;
}
////////////////////////////////////////////////////////////////////////

List TreeSearchBetween(Tree t, Record lower, Record upper) {
    List l = ListNew();
    doTreeSearchBetween(t, t->root, lower, upper, l);
    return l;
}

// n - the current node
// l - a list to accumulate results
static void doTreeSearchBetween(Tree t, Node n, Record lower,
    Record upper, List l) {
    // This function is transplanted from my lab work
    if (n == NULL) {
        return;
    }
    int cmp = t->compare(lower, n->rec);
    int cmp2 = t->compare(upper, n->rec);
    if (cmp < 0) {
        doTreeSearchBetween(t, n->left, lower, upper, l);
        if (cmp2 >= 0) {
            ListAppend(l, n->rec);
            doTreeSearchBetween(t, n->right, lower, upper, l);
        }
    } else if (cmp > 0) {
        doTreeSearchBetween(t, n->right, lower, upper, l);
    } else {  //cmp = 0, when cmp is equal to zero, there is no need to search left
              // so l list this situation separately.
        if (cmp2 >= 0) {
            ListAppend(l, n->rec);
            doTreeSearchBetween(t, n->right, lower, upper, l);
        }
    }
}


////////////////////////////////////////////////////////////////////////

Record TreeNext(Tree t, Record r) {
    Record next = NULL;
    doTreeNext(t, t->root, r, &next);
    return next;
}

static void doTreeNext(Tree t, Node n, Record r, Record *next) {
    //compexity O log n
    if (n == NULL) { return; }

    int cmp = t->compare(r, n->rec);
    if (cmp < 0) {
        *next = n->rec;
        doTreeNext(t, n->left, r, next);
    } else if (cmp > 0) {
        doTreeNext(t, n->right, r, next);
    } else {           //cmp = 0
        *next = n->rec;
    }
}
    

