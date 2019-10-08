/**
 *  Header file for a linked list in C
 *
 * @authors:   		Michael Denzel
 * @creation_date:	2016-09-05
 * @contact:		m.denzel@cs.bham.ac.uk
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// --- Data structures ---

// TODO: Define additional data structures here when you need them.

typedef struct list{
  // TODO: define your list structure here!
} list;

// --- Functions ---

/*
 * Function to initialize a new list
 *
 * #Argument
 * * `l`    - The list to be initialized
 *
 */
void init(list * l);

/*
 * Function to free a list when it is not needed anymore
 *
 * #Argument
 * * `l`    - The list to be destroyed
 *
 */
void destroy(list * l);

/*
 * Function to get `data` of element number `index`.
 * 
 * #Arguments
 * * `l`    - The list to be modified
 * * `index`   - the index of the searched element.
 *
 * #Return
 * Returns the `data` field of the element or -1 in case of errors.
 *
 */
int get(list * l, unsigned int index);

/*
 * Function to prepend a new element to the list.
 * 
 * #Arguments
 * * `l`    - The list to be modified
 * * `data`   - the integer to add to the front of the linkedlist.
 *
 * #Return
 * Returns 0 for successful termination and -1 in case of errors.
 *
 */
int prepend(list * l, int data);

/*
 * Function to append a new element to the list.
 * 
 * #Arguments
 * * `l`    - The list to be modified
 * * `data`   - the integer to add to the back of the linkedlist.
 *
 * #Return
 * Returns 0 for successful termination and -1 in case of errors.
 *
 */
int append(list * l, int data);

/*
 * Function to insert a new element to the list.
 * 
 * #Arguments
 * * `l`    - The list to be modified
 * * `index`  - the index after which the new element should be inserted.
 * * `data`   - the integer which should be stored in the new element.
 *
 * #Return
 * Returns 0 for successful termination and -1 in case of errors.
 *
 */
int insert(list * l, unsigned int index, int data);

/*
 * Function to delete an existing element from the list.
 * 
 * #Arguments
 * * `l`    - The list to be modified
 * * `index`   - the index of the element to remove from the linkedlist.
 *
 * #Return
 * Returns 0 for successful termination and -1 in case of errors.
 *
 */
int remove_element(list * l, unsigned int index);

/*
 * Function to print all elements of the list.
 *
 * #Arguments
 * * `l`      - the list to print.
 */
void print_list(list * l);

#endif
