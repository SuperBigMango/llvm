#include <stdio.h>
#include <stdint.h>
#include <assert.h>

void compare(int64_t index, int64_t upperBound) {
	printf("The index is %ld\t and the upper bound is %ld\n",index,upperBound);
	assert(index<upperBound  && "Array Out of bound exception");
}
