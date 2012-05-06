#ifndef _DLL_H_
#define _DLL_H_

/*Heap structure*/
typedef struct node_t {
	struct timeval time;
	struct node_t *left;
	struct node_t *right;
} node;

node *first; /*Start of the list*/
node *last; /*End of the list*/

node *dll_add_key(struct timeval *time);
int dll_extract_min(struct timeval *time);
void dll_min_key(struct timeval *time);

#endif /*end of _DLL_H_*/
