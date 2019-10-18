#include <stdlib.h>
#include <stdio.h>
#include "linkedlist.h"

void init(list *l)
{
    *l = *(list *) malloc(sizeof(list));
    l->head = NULL;
    l->last = NULL;
}

void destroy(list *l)
{
    printf("Destroy\n");
}

int get(list *l, unsigned int index)
{
    printf("Get\n");
    return 0;
}

int prepend(list *l, int data)
{
    listnode *n = (listnode *) malloc(sizeof(listnode));
    if (!n) return -1;

    n->data = data;

    if (!(l->head) && !(l->last)) {
        l->head = n;
        l->last = n;
        n->next = NULL;
    } else {
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
    printf("Insert\n");
    return 0;
}

int remove_element(list *l, unsigned int index)
{
    printf("Remove\n");
    return 0;
}

void print_list(list *l)
{
    if ((l->head) == NULL && (l->last) == NULL) {
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
