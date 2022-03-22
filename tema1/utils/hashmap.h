#include "list.h"
#include <sys/types.h>

typedef long(*HashFunction)(char*, int);

typedef struct {
	int hashmap_size;
  int hashmap_no_values;
	List *hashmap_slots;
	HashFunction hash_function;
}*HashMap, Hash;

long BASIC_HASH_FUNCTION(char*, int);

HashMap createHashMap(int, HashFunction);

void putInHashMap(HashMap, char*, char*);

char* valueOfKey(HashMap, char*);

int exists(HashMap, char*, char);

void freeHashMap(HashMap);

void printHashMap(HashMap);	
Node popFirst(HashMap);
