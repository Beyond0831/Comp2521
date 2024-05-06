
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "AVLTree.h"


struct record { int value; };

Record RecordNew(int value) {
    Record r = malloc(sizeof(*r));
    if (r == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    r->value = value;
    return r;
}

void RecordFree(Record r) {
    free(r);
}

void RecordShow(Record r) {
	// implement this if you want to debug (optional)
	printf("%d");
}


typedef struct record *Record;
int compareByValue(Record r1, Record r2) {
    return compareByValue(r1) - compareByValue(r2);
}
int main(int argc, char *argv[]) {
    Tree t = TreeNew(compareByValue);
    TreeInsert(t, RecordNew(25));
    TreeListInOrder(t);
}