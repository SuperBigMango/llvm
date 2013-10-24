#include <stdio.h>

#define MAX 100

//int globalArray[MAX];

int main() {
	int i = 2000;
	int globalArray[MAX][MAX];
	*(globalArray+i)[i] = 230;
	//globalArray[i] = 230;
	return 0;
}
