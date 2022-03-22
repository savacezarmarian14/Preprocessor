#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

Node newNode(char* value, char* key) {
	Node node = malloc(sizeof(sample_node));
	node->value = strdup(value);
	node->key = strdup(key);
	node->next = NULL;
	return node;
}
List createList() {
	List list = malloc(sizeof(List));
	
	list->head = NULL;
	
	return list;
}

int listSize(List list) {
	if(list->head == NULL)
		return  0;
	int len = 0;
	Node it = list->head;
	while(it != NULL) {
		len++;
		it = it->next;
	}
	return len;
}

int isEmpty(List list) {
	if(listSize(list) == 0)
		return 1;
	else
		return 0;
}
void printList(List list, const char* ptext) {
	Node it = list->head;
	while(it != NULL) {
		printf("%s", ptext);
		printf("%s\n", it->value);
		it = it->next;		
	}
}
	

void addElementToList(List list, Node nodeToAdd) {
	if(list->head == NULL)
		list->head = nodeToAdd;
	else {
		Node it = list->head;
		while(it->next != NULL)
			it = it->next;
		it->next = nodeToAdd;
	}
}

