// Written by Craig Swearingen for CSC389
// Introduction to Operating Systems - University of Illinois Springfield

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {MALE, FEMALE} person;

//struct node;

typedef struct node {
	struct node *previous;
	struct node *next;
	person data;
} node;

typedef struct List {
	struct node *first;
	struct node *last;
} List;

List *createLinkedList();

void freeLinkedList(List * a);

void pushLinkedList(List *a, person data);

node *removeLinkedList(List *a, person p);

node *removeFirstLinkedList(List *a);

void pushFirstLinkedList(List *l, node *a);

void printList(List *a);

bool isEmpty(List *a);

bool isNextMale(List *a);
