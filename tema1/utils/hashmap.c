#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"

long checksum(char* word) {
	long sum = 0;
	for (int i = 0; i < strlen(word); i++) {
		sum += (int) word[i];
	
	}
	return sum;
}

char *extractValue(HashMap h, char *key) {
	int index = h->hash_function(key, h->hashmap_size);
	Node it = h->hashmap_slots[index]->head;
	while(it != NULL) {
		if(strcmp(it->key, key) == 0)
			return strdup(it->value);
	}
	return NULL;
}

Node popFirst (HashMap h) {
 for(int i = 0; i < h->hashmap_size; i++) {
  if(h->hashmap_slots[i]->head != NULL) {
    Node it = h->hashmap_slots[i]->head;
    h->hashmap_slots[i]->head = h->hashmap_slots[i]->head->next;
    h->hashmap_no_values--;
    return it;
  }
 }
  return NULL; 
}

/* createHashMap( )
 * aloca spatiu pt hashmap
 */
void freeHashMap(HashMap h) {
	int i;
	for(i = 0; i < h->hashmap_size; i++) {
		Node it = h->hashmap_slots[i]->head;
		while(it != NULL) {
			Node aux = it;
			it = it->next;
			free(aux);
		}
	}
	free(h->hashmap_slots);
	free(h);
}

HashMap createHashMap(int size, HashFunction f) {
	HashMap h = malloc(sizeof(Hash));
	h->hashmap_size = size;
  h->hashmap_no_values = 0;
	h->hashmap_slots = malloc(size * sizeof(List));
	for (int i = 0; i < size; i++) {
		h->hashmap_slots[i] = createList();
	}
	h->hash_function = f;
}

/*  
 * exists( )
 *  verifica daca  keya/valoarea exista in functie 
 *  type  - 'v' 'k'
 */

int exists(HashMap h, char* key, char type) {
	for (int i = 0; i < h->hashmap_size; i++) {
		Node it = h->hashmap_slots[i]->head;
		while(it != NULL) {
			switch(tolower(type)) {
				case 'k' :
					if(strcmp(it->key, key) == 0)
						return 1;
					break;
				case 'v' :
					if(strcmp(it->value, key) == 0)
						return 1;
					break;
				default: 
					break;
			}	
			it = it->next;
		}
	}
	return 0;
}

void refreshHashMapValues(HashMap h, char* key, char* value) {
	for(int i = 0; i < h->hashmap_size; i++) {
		Node it = h->hashmap_slots[i]->head;
		
		//iterez in lista
		while(it != NULL) {
			if( strcmp(it->value, key) == 0 ) {
				it->value = strdup(value); // inlocuiesc cheia cu valoarea
			}
			it = it->next;
		}
	}
}

void putInHashMap(HashMap h, char* key, char* value) {
	int index = h->hash_function(key, h->hashmap_size);
	Node node = newNode(value, key);
	if(!exists(h, node->key, 'k')) {
		if(exists(h, node->value, 'k'))
			node = newNode(extractValue(h, node->value), key);
		else if(exists(h, node->key, 'v'))
			refreshHashMapValues(h, node->key, node->value);
		addElementToList(h->hashmap_slots[index], node);
    h->hashmap_no_values++;
	}
}
void printHashMap(HashMap h) {
	for (int i = 0; i < h->hashmap_size; i++) {
		printf( "[%d] :\n", i);
		printList(h->hashmap_slots[i], "\t\t");
	}
}
long BASIC_HASH_FUNCTION(char *word, int size) {
	return (int) checksum(word) % size;
}
char* valueOfKey(HashMap h, char* key) {
	int index = h->hash_function(key, h->hashmap_size);
	Node it = h->hashmap_slots[index]->head;
	while(it != NULL) {
		if(strcmp(key, it->key) == 0)
			break;
		it = it->next;
	}

	if(it == NULL)
		return NULL;
	else 
		return it->value;
}
 