#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

struct MemListNode  {
	void *mem;
	int64_t size;
	struct MemList *next;
};

typedef struct MemListNode Node;
static Node *tail = NULL;
static Node *head = NULL;
static int64_t counter = 0;

void malloc_hook(void *mem, int64_t size) {
	Node *node = (Node*) malloc(sizeof(Node));
	node->mem = mem;
	node->size = size;
	node->next = NULL;
	if(tail == NULL) {
		tail = node;
		head = node;
	}else {
		tail->next = node;
		tail = tail->next;
	}
	counter += size;
	fprintf(stderr,"MemoryFootPrint %lu\n",counter);
}

void free_hook (void *mem) {
	Node *n;
	for(n = head; n!= NULL; n = n->next) {
		if(n->mem == mem) {
			break;
		}
	}

	if(n != NULL) {
		counter -= n->size;
		fprintf(stderr , "MemoryFootPrint %lu\n",counter);
	}
}
