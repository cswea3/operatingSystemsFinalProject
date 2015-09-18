//Written by Craig Swearingen for CSC389
// Introduction to Operating Systems - University of Illinois Springfield

#include "linkedlist.h"

List *createLinkedList() {
	List *tmp =  malloc(sizeof(List));
	tmp->first = NULL;
	tmp->last = NULL; 
	return tmp;
}

void freeLinkedList(List *a) {
	node *next;
	node *current;
	
	if(!isEmpty(a)){
		current = (node *) a->first;
		do{
			next = (node *)current->next;
			free(current);
			current = next;
		}while(next != NULL);
	}	
	free(a);
}

void pushLinkedList(List *a, person data) {
	node *newNode = malloc(sizeof(node));
	newNode->data = data;
	newNode->next=NULL;

	if(a->last != NULL){
		a->last->next = newNode;
		newNode->previous = a->last;
		a->last = newNode;
	}else{
		a->first = newNode;
		a->last=newNode;
	}
}

void pushFirstLinkedList(List *l, node *a) {
	node *temp = l->first;
	l->first = a;
	a->next = temp;
}

node *removeLinkedList(List *a, person p){
	

	if(a->first == NULL) {
		return a->first;
	}
	
	if(a->first->next == NULL && a->first->data == p) {
		node *t = a->first;
		a->first = NULL;
		a->last = NULL;
		return t;
	}
	node *tmp = a->first;
	while(tmp != NULL && tmp->data != p ){
			tmp=tmp->next;
	}
	
	if(tmp == a->first && tmp->next!= NULL){
		a->first = tmp->next;
		return tmp;
	}

	if(tmp != NULL) {
		tmp->previous->next = tmp->next;
		if(tmp->next != NULL) {
			tmp->next->previous = tmp->previous;
		}
	}

	return tmp;

}

node *removeFirstLinkedList(List *a) {
	if(a->first == NULL) {
		return a->first;
	}
	
	if(a->first->next == NULL) {
		node *t = a->first;
		a->first = NULL;
		a->last = NULL;
		return t;
	}

	node *tmp = a->first;
	a->first = a->first->next;
	return tmp;
}

void printList(List *l){
	node *test;
	test = (node *)l->first;
	while(test != NULL){
		if(test->data == 0) {
			printf("M ");
		}else {
			printf("F ");
		}
		test = test->next;
	}
	printf("\n");
}

bool isEmpty(List *a) {
	return (a->first == NULL);
}
bool isNextMale(List *a) {
	if(a->first != NULL)
		return (a->first->data == MALE);

	return NULL;
}
