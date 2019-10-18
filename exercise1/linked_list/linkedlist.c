#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

void init(list *l)
{
    l->head = NULL;
    l->last = NULL;
}

void destroy(list *l)
{
    listnode *n = l->head;
    listnode *m;

    // Traverse the list and free each listnode in turn
    while (n) {
        m = n;
        n = n->next;
        free(m);
    }

    l = NULL;
}

int get(list *l, unsigned int index)
{
    // List is empty
    if (!(l->head) && !(l->last)) return -1;

    listnode *n = l->head;

    // Traverse the list until we get to the correct index
    while (n) {
        if (index == 0) return n->data;
        n = n->next;
        index--;
    }

    // Something went wrong, return error
    return -1;
}

int prepend(list *l, int data)
{
    listnode *n = (listnode *) malloc(sizeof(listnode));
    if (!n) return -1;

    n->data = data;

    if (!(l->head) && !(l->last)) {
        // If the list is empty, set node to both the head and the last of the list
        l->head = n;
        l->last = n;
        n->next = NULL;
    } else {
        // Otherwise, replace the head of the list with the new node
        n->next = l->head;
        l->head = n;
    }

    return 0;
}

int append(list *l, int data)
{
    listnode *n = (listnode *) malloc(sizeof(listnode));
    if (!n) return -1;

    n->next = NULL;
    n->data = data;

    if (!(l->head) && !(l->last)) {
        // If the list is empty, set the node to both the head and the last of the list
        l->head = n;
        l->last = n;
    } else {
        // Otherwise, append the node to the last of the list
        l->last->next = n;
        l->last = n;
    }

    return 0;
}

int insert(list *l, unsigned int index, int data)
{
    // List is empty
    if (!(l->head) && !(l->last)) return -1;

    listnode *p = l->head;

    // Traverse the list until correct index is reached
    while (p) {
        if (index == 0) {
            // Allocate memory for the new list node
            listnode *n = (listnode *) malloc(sizeof(listnode));
            if (!n) return -1;

            // Insert the new node in between the current and next nodes
            n->next = p->next;
            n->data = data;
            p->next = n;

            return 0;
        }
        p = p->next;
        index--;
    }

    // Something went wrong, return error
    return -1;
}

int remove_element(list *l, unsigned int index)
{
    // List is empty
    if (!(l->head) && !(l->last)) return -1;

    listnode *n = l->head;
    unsigned int i = index;
    unsigned int j = 0;
    listnode *last = NULL;

    // Traverse the list until the correct index is reached
    while (n) {
        if (i == 0) {
            if (last) last->next = n->next;
            if (j == 0) l->head = n->next;
            if (j == index) l->last = last;
            free(n);
            return 0;
        }
        last = n;
        n = n->next;
        i--;
        j++;
    }

    // Something went wrong, return error
    return -1;
}

void print_list(list *l)
{
    if (!(l->head) && !(l->last)) {
        // Empty list
        printf("empty list\n");
    } else {
        // Non-empty list
        listnode *n = l->head;
        while (n) {
            printf("%d ", n->data);
            n = n->next;
        }
        printf("\n");
    }
}
