#ifndef _MINHEAP_H_
#define _MINHEAP_H_

#define TIME int

typedef struct node_t {
	TIME time;
	struct node_t *left;
	struct node_t *right;
    struct node_t *parent;
} node;

node *root, *last;
int node_index = 1; /* Represents the index of the current node where 
                * leaf is to be added
                */
int base_index = 1; /* Nearest power of 2 lower than the index*/

int add_node(TIME time);
TIME extract_min();
TIME read_min();

#endif /*end of _MINHEAP_H_*/
