#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <inttypes.h>

#include <map>
using namespace std;

static int64_t counter  = 0 ;

map

void malloc_hook(void *p,int64_t num) {
	counter += num;
}


void free_hook(void *p) {
	counter -= mem_size;
}
