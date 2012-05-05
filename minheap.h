#ifndef _MINHEAP_H_
#define _MINHEAP_H_
/*For the data to be stored in heap*/
#define TIME int

/*Heap structure*/
typedef struct node_t {
	TIME time;
	struct node_t *left;
	struct node_t *right;
    struct node_t *parent;
} node;

node *root; /*Maintain root of the heap*/
node *last; /* Maintain the current element of the heap to which
             * a leaf was added*/

int node_index = 1; /* Represents the index of the current node where 
                * leaf is to be added
                */
int base_index = 1; /* Nearest power of 2 lower than the index*/

inline int isleft(node *t)
{
    if(t && t->parent) {
        if(t->parent->left  == t) {
            return 1;
        }
    }
    return 0;
}
inline int isright(node *t)
{
    if(t && t->parent) {
        if(t->parent->right  == t) {
            return 1;
        }
    }
    return 0;
}

inline int isleaf(node *t)
{
    if(t && !t->left && !t->right) {
        return 1;
    }
    return 0;
}

inline int isroot(node *t)
{
    if(t && !t->parent) {
        return 1;
    }
    return 0;
}

inline void preorder(node *t) {
    if(!t) {
        return;
    }
    printf("%d .. ", t->time);
    preorder(t->left);
    preorder(t->right);
}
    
int mh_add_key(TIME time);
TIME mhextract_min();
TIME mh_min_key();

#endif /*end of _MINHEAP_H_*/
