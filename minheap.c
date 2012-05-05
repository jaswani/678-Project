#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include "debug.h"
#include "minheap.h"
/* Objective : Use a min-heap to maintain delta list
 * better than link list in efficiency and using same
 * amount of storage
 * Edit: need to cache parent to storage is increased 
 * by one pointer size.
 * Logic: A min heap to be efficient should be grown in
 * the form of a almost complete binary tree, fill all 
 * at each level. To realise that we try and fit the array
 * representation of a tree on to the pointer implementation
 * the array index is then used to find out the path to be 
 * taken to get to the appropriate node.
 * 
 * 04/05/2012: This might not be the most efficient way of implementing
 * min heap as if my analsyis is correct cost of n insertions to 
 * heap using this approach is O(nlgn) while using back pointers it
 * is O(n+lgn). Will try to optimize this later..
 */


/* mh_cr_node: create a new node
 */
node *mh_cr_node (TIME time)
{
    node *timer = NULL;
    timer = malloc(sizeof(struct node_t));
    if(!timer) {
        /*malloc failure*/
        DEBUG_NA("Error: Could not allocate memory\n");
        return NULL;
    }
    timer->time = time;
    timer->left = timer->right = timer->parent = NULL;
    return timer;
}

/* 
 * mh_del_node: delete a node
 */
int mh_del_node(node *t)
{
    if(t->left || t->right) {
        DEBUG_NA("ERROR: Attempt to delete non leaf node\n");
        return 0;
    }

    if(!t->parent) {
        DEBUG_NA("Deleting root..heap empty!\n");
        free(t);
        root = last = NULL;
        return 1;
    }

    if(isleft(t)) {
        t->parent->left = NULL;
    } else {
        t->parent->right = NULL;
    }

    t->left = t->right = t->parent = NULL;

    free(t);
    return 1;
}

/*
 * mh_inc_key : increase the key of node t to the new value
 */
int mh_inc_key(node *t, TIME new_time)
{
    if(!t) {
       DEBUG_NA("Error: NULL agrument to mh_inc_key");
        return 0;
    }
    while(t->parent) {
        if(t->parent->time > new_time) {
            TIME temp = t->parent->time;
            t->parent->time = new_time;
            t->time = temp;
            t = t->parent;
        } else {
            break;
        }
    }
    return 1;
}

/*
 * mh_heapify: ensure the heap property is restored for
 * the subtree rooted at node t
 * Can also be used for decrease key..just a thought
 */
int mh_heapify(node *t)
{
   node *temp = t;
   TIME p;
   if(!t) {
       DEBUG_NA("Error: NULL agrument to mh_heapify");
       return 0;
   }
   while(1) {
       if(t->left) {
           if(t->left->time < t->time) {
               temp = t->left;
           } 
           
           if( t->right && (t->right->time < temp->time)) {
               temp = t->right;
           }
       }
       
       if(temp != t) {
           p = temp->time;
           temp->time = t->time;
           t->time = p;
           t = temp;
       } else {
           break;
       }
   }
}

/*
 * mh_find_next: Find the next node to which
 * new leaves can be added
 */
node *mh_find_next(node *t)
{
    int flag = 1, found_left_ancestor = 0;
    while(flag && t->parent) {
        if(isleft(t)) {
            /* t is the left child 
             * Last iteration break after this
             */
            flag = 0;
            found_left_ancestor = 1;
        }
        t = t->parent;
    }

    if(found_left_ancestor) {
        /*go once to right*/
        t = t->right;
    }

    while(!isleaf(t)) {
        t = t->left;
    }

    return t;
}
 
/*
 * mh_find_prev: Find the prev node to which
 * new leaves were last added
 */
node *mh_find_prev(node *t)
{
    int flag = 1, found_right_ancestor = 0;
    while(flag && t->parent) {
        if(isright(t)) {
            /* t is the right child 
             * Last iteration break after this
             */
            flag = 0;
            found_right_ancestor = 1;
        }
        t = t->parent;
    }

    if(found_right_ancestor) {
        /*go once to left*/
        t = t->left;
    }

    while(!isleaf(t)) {
        t = t->right;
    }
    /*previous is non leaf*/
    if(!isroot(t)) {
        t = t->parent;
    }
    return t;
}

/*
 * mh_add_key: Add a new key to the heap
 */
int mh_add_key(TIME time)
{
    node *timer = NULL;
    node *iter = root;
    int i;
    timer = mh_cr_node(time);
    if(!timer) {
        return 0;
    }

    if(!root) {
        /*empty heap*/
        root = last = timer;
    } else {
        if(last->right) {
            /*This last is full find next*/
            last = mh_find_next(last);
        }
        
        
        if(!last->left) {
            /*Left slot is empty*/
            last->left = timer;
        } else {
            last->right = timer;
        }
        timer->parent = last;
    }
    /*Success*/
    mh_inc_key(timer, time);
    return 1;
}

/*
 * mh_remove_key: Remove a key to the heap
 */
int mh_remove_key(node *timer)
{
    node *iter = last;
    int flag = 0;

    if(!isleaf(last)) {
        /*More than one element in the list*/
        if(last->right) {
            iter = last->right;
        } else {
            iter = last->left;
            flag = 1;
        }
        timer->time = iter->time;
    }
    
    mh_del_node(iter);
    if(flag && last) {
        /* Now last has become a leaf
         * find the prev non leaf last
         */
        //DEBUG("last before = %d\n", last->time);
        last = mh_find_prev(last);
      //  DEBUG("last after = %d\n", last->time);
    }
    
    mh_heapify(timer);
    //DEBUG("last after = %d\n", last->time);
}

/*
 * mh_extract_min: Extract min key to the heap
 */
TIME mh_extract_min()
{
    TIME ret_val = root->time;
    mh_remove_key(root);
    return ret_val;
}

/*
 * mh_min_key: Return data of the min key
 */
TIME mh_min_key()
{
    return root->time;
}
   
main(int argc, char *argv[]) 
{ 
    TIME value;
    int i, iterations;
    
    if(argc == 1) {
        printf("Usage ./a.out <no of elements> \n");
        return;
    } else {
        iterations = atoi(argv[1]);
        DEBUG("iterations =  %d\n", i);
    }
    
    printf("Inserting: \n");

    for(i=0; i<iterations;i++) {
        value = rand()%1000;
        printf("%d ..", value);
        mh_add_key(value);
    }

    printf("\nExtracting: \n");
    
    while(i--) {
        /*printf("\nPreorder: ");
        preorder(root);
        printf("\n");*/
        value = mh_extract_min();
        printf("%d ..", value);
    }
    
    printf("\n");
}
    
    
    
