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
 */


/* create_node: create a new timer for a packet.
 */
node *create_node (TIME time)
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

int del_node(node *t)
{
    if(t->left || t->right) {
        DEBUG_NA("ERROR: Attempt to delete non leaf node");
        return 0;
    }

    if(t->parent->left == t) {
        t->parent->left = NULL;
    } else {
        t->parent->right = NULL;
    }

    t->left = t->right = t->parent = NULL;

    free(t);
}

int inc_node(node *t, TIME new_time)
{
    if(!t) {
       DEBUG_NA("Error: NULL agrument to inc_node");
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

int heapify(node *t)
{
   node *temp = t;
   TIME p;
   if(!t) {
       DEBUG_NA("Error: NULL agrument to heapify");
       return 0;
   }
   while(1) {
       if(t->left) {
           if(t->left->time < t->time) {
               temp = t->left;
           } else if( t->right && (t->right->time < temp->time)) {
               temp = t->right;
           }
       }
       
       if(temp != t) {
           p = temp->time;
           temp->time = t->time;
           t->time = p;
           temp = t;
       } else {
           break;
       }
   }
}


int add_node(TIME time)
{
    node *timer = NULL;
    node *iter = root;
    int i;
    timer = create_node(time);
    if(!timer) {
        return 0;
    }

    if(!root) {
        /*empty heap*/
        root = timer;
    } else {
        /* Heap is not empty now we get into the index logic.
         * Basically index tells the current node to which new
         * leaf can be added.
         */
         for(i = base_index>>1; i > 0; i = i>>1) {
             if(!iter) {
                 /*Error*/
                 DEBUG_NA("Error: iter becomes NULL in for\n");
                 free(timer);
             }
             if (i & node_index) {
                iter = iter->right;
             } else {
                iter = iter->left;
             }
         }

         /* Now iter should point to the node where this new node
          * can be added
          */

         if(!iter || (iter->left && iter->right)) {
             /*Something has gone wrong*/
             DEBUG_NA("Error: iter is NULL\n");
             free(timer);
             return 0;
         }

         if(!iter->left) {
            iter->left = timer;
         } else {
            iter->right = timer;
            node_index++;
            if(node_index > base_index) {
                base_index = base_index << 1;
            }
         }
         timer->parent = iter;
    }
    /*Success*/
    //FIXME: Call to increase key needs to go here..
    inc_node(timer, time);
    return 1;
}        
    
TIME extract_min()
{
    node *iter = root;
    TIME ret_val = root->time;
    int i;
    for(i = base_index>>1; i > 0; i = i>>1) {
        if(!iter) {
            /*Error*/
            DEBUG_NA("Error: iter becomes NULL in for\n");
        }
        if (i & node_index) {
            iter = iter->right;
        } else {
            iter = iter->left;
        }
    }

    if(iter->left) {
        iter = iter->left;
    }

    root->time = iter->time;
    del_node(iter);
    if(node_index > 1) {
        node_index--;
    } else {
        root = NULL;
    }
    
    if((base_index >> 1 > node_index)) {
        base_index = base_index >> 1;
    }

    heapify(root);
    return ret_val;
}

TIME read_min()
{
    return root->time;
}
   
main(int argc, char *argv[]) 
{ 
    if(argc == 1)
        DEBUG_NA("This is dummy function \n");
    else 
        DEBUG("This is dummy function %d %s\n", argc, argv[1]);
}
    
    
    
