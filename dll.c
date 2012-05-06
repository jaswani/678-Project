#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include "debug.h"
#include "dll.h"
#include "timer_utils.h"

/* mh_cr_node: create a new node
 */
node *dll_cr_node (struct timeval *time)
{
    node *t = NULL;
    t = malloc(sizeof(struct node_t));
    if(!t) {
        /*malloc failure*/
        DEBUG_NA("Error: Could not allocate memory\n");
        return NULL;
    }
    t->time.tv_sec = time->tv_sec;
    t->time.tv_usec = time->tv_usec;
    t->left = t->right = NULL;
    return t;
}

/* 
 * mh_del_node: delete a node
 */
int dll_del_node(node *t)
{
    if(!t) {
        DEBUG_NA("ERROR: Null argument to del_node\n");
        return 0;
    }

    if(t == first) {
        DEBUG_NA("Deleting first node!\n");
        first = first->right;
        if(!first) {
            DEBUG_NA("List empty!\n");
            last = NULL;
        } else {
            first->left = NULL;
        }
    } else if(t == last) {
       t->left->right = t->right;
       last = t->left;
    } 
    else {
        t->left->right = t->right;
        t->right->left = t->left;
    }

    t->left = t->right = NULL;

    free(t);
    return 1;
}

/*
 * dll_add_key: Add a new key to the heap
 */
node* dll_add_key(struct timeval *time)
{
    node *t = NULL;
    node *iter = first;
    int i;
    t = dll_cr_node(time);
    if(!t) {
        return NULL;
    }
    
    if(!first) {
        /*Empty List*/
        first = last = t;
    } else {
        while(iter && (timeval_compare(time, &iter->time) == 1)) {
            /*time is greater than current node*/
            iter = iter->right;
        }
        /*We are now at the node where new node is to be insterted */
        if(!iter) {
            /*Insert at the end*/
            last->right = t;
            t->left = last;
            last = t;
        } else {
           /*Insert before iter*/
           t->left  = iter->left;
           t->right = iter;
           if(t->left) {
               /*First node doesn't have left*/
               t->left->right = t;
           } else {
               /*Update first*/
               first = t;
           }
           t->right->left = t; 
        }
    }
    /*Success*/
    return t;
}

/*
 * mh_remove_key: Remove a key to the heap
 */
int dll_remove_key(node *t)
{
    return dll_del_node(t);
}

/*
 * dll_extract_min: Extract min key to the heap
 */
int dll_extract_min(struct timeval *time)
{
    time->tv_sec = first->time.tv_sec;
    time->tv_usec = first->time.tv_usec;
    dll_del_node(first);
    return 1;
}

/*
 * dll_min_key: Return data of the min key
 */
void dll_min_key(struct timeval *time)
{
    time->tv_sec = first->time.tv_sec;
    time->tv_usec = first->time.tv_usec;
}

#if 0
main(int argc, char *argv[]) 
{ 
    struct timeval value;
    int i, iterations;
    
    value.tv_sec = 0;
    value.tv_usec = 0;
    
    if(argc == 1) {
        printf("Usage ./a.out <no of elements> \n");
        return;
    } else {
        iterations = atoi(argv[1]);
        DEBUG("iterations =  %d\n", iterations);
    }
    
    printf("Inserting: \n");

    for(i=0; i<iterations;i++) {
        value.tv_sec = rand()%1000;
        printf("%d ..", value.tv_sec);
        dll_add_key(&value);
    }

    printf("\nExtracting: \n");
    
    while(i--) {
        /*printf("\nPreorder: ");
        preorder(root);
        printf("\n");*/
        dll_extract_min(&value);
        printf("%d ..", value.tv_sec);
    }
    
    printf("\n");
}
#endif    
