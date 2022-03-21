#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.h"
int main() {
	
	HashMap h = createHashMap(10, &BASIC_HASH_FUNCTION);
	putInHashMap(h, "b", "123");
	putInHashMap(h, "b", "234");
	printf("%d, %d\n", exists(h, "b", 'k'), 1);
	printHashMap(h);
	return 0;
}
