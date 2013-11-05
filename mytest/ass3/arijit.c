#include <stdio.h>

int main () {
	char *mem = (char *)malloc(10);
	if(mem) {
		printf("Memory allocation is successful\n");
		free(mem);
	}else {
		printf("Unsuccessful\n");
	}

	return 0;
}
