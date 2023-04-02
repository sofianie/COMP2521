// Implementation of the Tree ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Record.h"
#include "Tree.h"

typedef struct node *Node;
struct node {
    Record rec;
    Node   left;
    Node   right;
    // IMPORTANT: Do not modify the fields above
    // You may add additional fields below if necessary
    int height;
};

struct tree {
    Node root;
    int (*compare)(Record, Record);
    // IMPORTANT: Do not modify the fields above
    // You may add additional fields below if necessary
};

Tree TreeNew(int (*compare)(Record, Record));
void TreeFree(Tree t, bool freeRecords);
static void doTreeFree(Node n, bool freeRecords);
bool TreeInsert(Tree t, Record rec);
Record TreeNext(Tree t, Record rec);
List TreeSearchBetween(Tree t, Record lower, Record upper); 
Record TreeSearch(Tree t, Record rec);
// Added functions
static Node doTreeInsert(Tree t, Node n, Record rec, bool *result);
int height(Node n);
int max(int a, int b);
static Node rotate_left(Node n);
static Node rotate_right(Node n);
static Node newNode(Record rec);
static Record doTreeSearch(Tree t, Node n, Record rec);
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l);
static Record doTreeNext(Tree t, Node n, Record rec);

////////////////////////////////////////////////////////////////////////
// Provided functions
// !!! DO NOT MODIFY THESE FUNCTIONS !!!

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
// Functions you need to implement

// Returns TRUE if the record was inserted into the tree
// FALSE if the tree already contains a record that compares equal to the given record.
bool TreeInsert(Tree t, Record rec) {
    bool result = false;
    t->root = doTreeInsert(t, t->root, rec, &result);
    return result;
}

// Returns and calculates height 
int height(Node n) {
    if (n == NULL) {
        return 0;
    }
    return n->height;
}
 
// Returns the bigger integer 
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Performs left rotation (logic from balancing BSTs lecture pseudocode)
static Node rotate_left(Node n2) {
    Node n1 = n2->right; 
    n2->right = n1->left;
    n1->left = n2;

    n2->height = max(height(n2->left), height(n2->right)) + 1;
    n1->height = max(height(n1->left), height(n1->right)) + 1;
    return n1;
}

// Performs right rotation (logic from balancing BSTs lecture pseudocode)
static Node rotate_right(Node n1) {
    Node n2 = n1->left;
    n1->left = n2->right;
    n2->right = n1;

    n1->height = max(height(n1->left), height(n1->right)) + 1;
    n2->height = max(height(n2->left), height(n2->right)) + 1;
    return n2;
}

// Performs the actual insertion in tree and rotates to balance
static Node doTreeInsert(Tree t, Node n, Record rec, bool *result) {
    // Heavily based off of code from lab04 Tree.c
    if (n == NULL) {
        *result = true;
        return newNode(rec);
    }
    // Compares between record of current node and record to be inserted
    int cmp = t->compare(rec, n->rec);
    if (cmp < 0) {
        n->left = doTreeInsert(t, n->left, rec, result);
    } else if (cmp > 0) {
        n->right = doTreeInsert(t, n->right, rec, result);
    } else {
        *result = false;
    }
    // Following code makes tree self-balancing
    // (based off of pseuodocode from AVL Trees lecture code)
    if (n->left != NULL && n->right != NULL) { 
        int leftcmp = t->compare(rec, n->left->rec);
        int rightcmp = t->compare(rec, n->right->rec);

        n->height = height(n);
        int balance = height(n->left) - height(n->right);
        // checks if tree is balanced – if not, rotate
        if (balance > 1) {
            if (leftcmp > 0) {
                n->left = rotate_left(n->left);
                return rotate_right(n);
            } else if (leftcmp < 0) {
                return rotate_right(n);
            }
        } else if (balance < -1) {
            if (rightcmp < 0) {
                n->right = rotate_right(n->right);
                return rotate_left(n);
            } else if (rightcmp > 0) {
                return rotate_left(n);
            }
        } 
    }
    return n;
}

// Copied code from Tree.c in lab04
// Creates a new node after searching once there is a NULL space in a tree
static Node newNode(Record rec) {
    Node n = malloc(sizeof(*n));
    if (n == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    n->left = NULL;
    n->right = NULL;
    n->rec = rec;
    n->height = 0; 
    return n;
}

Record TreeSearch(Tree t, Record rec) {
    return doTreeSearch(t, t->root, rec);
}

// Copied code from Tree.c in lab04
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

List TreeSearchBetween(Tree t, Record lower, Record upper) {
    List l = ListNew();
    doTreeSearchBetween(t, t->root, lower, upper, l);
    return l;
}

// n - the current node
// l - a list to accumulate results
static void doTreeSearchBetween(Tree t, Node n, Record lower,
                                Record upper, List l) {
    if (n == NULL) {
        return;
    }
    // Copied code from Tree.c in lab04
    int lowercmp = t->compare(n->rec, lower);
    int uppercmp = t->compare(n->rec, upper);

    if (lowercmp >= 0 && (uppercmp <= 0)) {
        if (n->left != NULL) {
            doTreeSearchBetween(t, n->left, lower, upper, l);
        }
        ListAppend(l, n->rec);
        if (n->right != NULL) {
            doTreeSearchBetween(t, n->right, lower, upper, l);
        }
    } else if (lowercmp < 0) {
        if (n->right != NULL) {
            doTreeSearchBetween(t, n->right, lower, upper, l);
        }
    } else if (uppercmp > 0) {
        if (n->left != NULL) {
            doTreeSearchBetween(t, n->left, lower, upper, l);
        }
    }
}

// Return smallest record that is >= rec or NULL if there is none 
Record TreeNext(Tree t, Record rec) {
    Record smallestbigrec = doTreeNext(t, t->root, rec);
    return smallestbigrec;
}

static Record doTreeNext(Tree t, Node n, Record rec) {
    if (n == NULL) {
        return NULL;
    }
    // Compares current node with given record, if n->rec is bigger, cmp > 0
    int cmp = t->compare(n->rec, rec);

    if (cmp < 0) {
        // If there's no other record bigger than rec then return NULL
        if ((n->right) == NULL) {
            return NULL;
        }
        int rightcmp = t->compare(n->right->rec, rec);
        // If right node of curr is bigger and there's no nunber thats smaller than that then 
        // return that node record
        if (rightcmp > 0 && (n->right->left) == NULL) {
            return n->right->rec;
        }
        Record r = doTreeNext(t, n->right, rec);
        if (r == NULL) {
            return NULL;
        } else {
            return r;
        }
    } else if (cmp > 0) {
        // current node record is bigger
        if ((n->left) == NULL) {
            // if there is no more smaller bigger value, return the current value
            return n->rec;
        }
        int leftcmp = t->compare(n->left->rec, rec);
        if ((leftcmp < 0) && n->left->right == NULL) {
            return n->rec;
        } 

        Record r = doTreeNext(t, n->left, rec);
        if (r == NULL) {
            return n->rec;
        } else {
            return r;
        }
    } else if (cmp == 0) {
        // if records are the same, return the current node
        return rec;
    }
    return NULL; 
}
////////////////////////////////////////////////////////////////////////
