typedef struct Node {
	char* value;
	char* key;
	struct Node* next;
}*Node, sample_node;

typedef struct List {
 	Node head;
}*List;

Node newNode(char*, char*); /* create a new node with value=value */
List createList(); /* creates a new list with head = NULL */
void addElementToList(List, Node nodeToAdd); /* adds nodeToAdd in the back of the list */
int listSize(List);
int isEmpty(List list);
void printList(List, const char*);


