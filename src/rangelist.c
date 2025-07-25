#include <stdlib.h>
#include <stdio.h>
#include "rangelist.h"

// Appends a range to a linked list
void add(Range r, RangeList *lst) {
    Node *new = malloc(sizeof(Node));
    new->range = r;
    new->next = NULL;

    if (lst->head == NULL) {
        lst->head = new;
        lst->len = 1;
        return;
    }
    Node *temp = lst->head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new;
    lst->len++;
}

// I LITERALLY FORGOR WHY WE'RE TAKING DOUBLE POINTER INSTEAD OF SINGLE

// Frees the elements in the linked list and initializes the head to nullptr
void clear(RangeList *list) {
    Node *temp = list->head;
    // wait i kinda forgor how to do this, wtf
    while (temp != NULL) {
        // Stop freeing when temp reaches a null or smth, idk
        Node *toFree = temp;
        // Go to the next node and THEN free the current node, else it will segfault
        temp = temp->next;
        free(toFree);
    }
    // Reinitialization for safety and convenience reasons
    list->head = NULL;
    list->len = 0;
}

// A handy display function I kept just in case
void display(RangeList *n) {
    Node *temp = n->head;
    while (temp != NULL) {
        Range curR = temp->range;
        printf("(%d, %d), ", curR.startInd, curR.endInd);
        temp = temp->next;
    }
    printf("\nmama mia!\n");
}