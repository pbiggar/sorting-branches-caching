#include <stdlib.h>
#include <stdio.h>
#include "base_sorts.h"
#include "cache_sorts.h"
#include "branch_sorts.h"
#include "on2_sorts.h"

#ifndef SORT
#error "You must define the sort function like this: \"gcc -DSORT=base_insertion\""
#endif

#ifndef RANDOM_SIZE
#error "You must define the size of the array like this: \"gcc -DRANDOM_SIZE=128\""
#endif

#ifndef SEED
#error "You must define the size of the array like this: \"gcc -DRANDOM_SIZE=128\""
#endif

int main()
{
	/* if you segfault here, comment out _FASTSORT_STACK */
	/* this happens because your system stack ownt take a 2 or 4meg stack variable */
/*#define _FASTSORT_STACK */
#ifdef _FASTSORT_STACK
	unsigned int random_array[RANDOM_SIZE];
#else
	unsigned int* random_array = malloc(RANDOM_SIZE * sizeof(unsigned int));
#endif
	int i;
	srand(SEED);
	for(i = 0; i < RANDOM_SIZE; i++)
	{
		random_array[i] = rand();
	}
	SORT(random_array, RANDOM_SIZE);

#ifndef _FASTSORT_STACK
	free(random_array);
#endif
	return 0;

}


