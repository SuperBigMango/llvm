#include <stdio.h>
#include <malloc.h>
#include <stdint.h>

static int64_t counter  = 0 ;

void malloc_hook(int64_t num) {
	printf("The number is %ld\n",num);
	counter += num;
}


void free_hook(void *p) {
	int64_t mem_size = malloc_usable_size(p);
	printf("Deallocating %ld\n",num);
	counter -= mem_size;
}
