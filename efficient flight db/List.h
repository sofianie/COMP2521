// A List ADT for records

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#include "Record.h"

typedef struct list *List;

// Creates a new empty list
// Complexity: O(1)
List ListNew(void);

// Frees the given list, but not the records contained in the list
// Complexity: O(1)
void ListFree(List l);

// Returns the number of items in the list
// Complexity: O(1)
int  ListSize(List l);

// Adds a record to the end of the list. Does NOT make a copy of the
// record before adding it to the list.
// Amortised complexity: O(1)
// Note: You can think of 'amortised complexity' as 'guaranteed average
//       complexity'
void ListAppend(List l, Record r);

// Extends the list `l` with the records from the list `add`.
// Amortised complexity: O(m), where m is the size of list `add`.
void ListExtend(List l, List add);

////////////////////////////////////////////////////////////////////////
// Do NOT use these functions

typedef struct listIterator *ListIterator;

// Creates an iterator for the given list
// Complexity: O(1)
ListIterator ListItNew(List l);

// Gets the next item in the list. The item should not be modified.
// Complexity: O(1)
Record ListItNext(ListIterator it);

// Checks if the list has a next item
// Complexity: O(1)
bool ListItHasNext(ListIterator it);

// Frees the given iterator
// Complexity: O(1)
void ListItFree(ListIterator it);

#endif
