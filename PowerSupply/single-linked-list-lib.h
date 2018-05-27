#ifndef SLL_H_
#define SLL_H_

// @sllnode
// This is struct of a node:
// *data point to node's data
// *next point to next node
//
typedef struct sllnode {
    void *data;
    struct sllnode *next;
} sllnode;

// @sll
// This is bone of single linked list:
// *head point to first node
// size is the number of nodes, not size in memory
//
typedef struct sll {
    struct sllnode *head;
    int size;
} sll;

// create empty list, return NULL if fail
sll* sll_create(void);

// destroy a list and free memory
void sll_destroy(sll* list);

// create new node with data = data
sllnode *sll_new_node (void *data);

// return node at index i (start from 1)
sllnode *sll_get_node (sll *list, int i);

// get last node
sllnode *sll_get_last_node (sll *list);

// get node before a node
sllnode *sll_get_prev_node (sll *list, sllnode *node);

// print out the list
// You should write a print function to print one node and pass it as function argument
// Remember to cast 'void *data' to your desired data type
// Example print function:
//
// void print_data (sllnode *node) {
//   my_data_type *my_data = (my_data_type*) node->data   //casting
//   .......... <print my_data>............
// }
int sll_traverse(sll* list, void (*get_data)(sllnode* node));

// return a node which store the given data
// You should write a compare function for one node and pass it as function argument
// search_data should return 1 on match, 0 otherwise
sllnode *sll_search(sll *list, int (*search_data)(sllnode* node));

// add a node to first position
int sll_prepend (sll *list, sllnode *node);

// add a node to last position
int sll_append (sll *list, sllnode *node);

// add a node after previous node
int sll_insert_after (sll *list , sllnode *prev, sllnode *node);

// add a node before next node
int sll_insert_before (sll* list, sllnode *nxt, sllnode *node);

// delete and return first node of the list
sllnode *sll_pop_head (sll* list);

// delete and return last node of the list
sllnode *sll_pop_tail (sll* list);

//delete and return a node in list
sllnode *sll_pop (sll* list, sllnode *node);

// reverse a list
sll *sll_reverse (sll* list);

#endif
