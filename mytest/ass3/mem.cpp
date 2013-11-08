#include <stdio.h>
#include <malloc.h>


int main()
{
	char *x=(char*)malloc(1234);
	printf( "%lu\n", malloc_usable_size(x) );
	return 0;
}
