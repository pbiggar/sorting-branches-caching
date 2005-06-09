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

#undef _USE_SOFTWARE_PREDICTOR
int main()
{
	/* if you segfault here, comment out _FASTSORT_STACK */
	/* this happens because your system stack ownt take a 2 or 4meg stack variable */
#define _FASTSORT_STACK
#ifdef _FASTSORT_STACK
	unsigned int random_array[RANDOM_SIZE];
#else
	unsigned int* random_array = malloc(RANDOM_SIZE * sizeof(unsigned int));
#endif

	/* fill the array or print error message */
	int count = 0;
	int random_file = open("paper/BIG", O_RDONLY);
	if (random)
	{
		count = read(random_file, random_array, sizeof(unsigned int) * RANDOM_SIZE);
	}
	if (count < RANDOM_SIZE)
	{
		fprintf(stderr, "Random not inited\n"); /* does printing to stderr stop simple? */
		fprintf(stdout, "Random not inited\n"); /* do this just in case */
	}
	close(random_file);




	SORT(random_array, RANDOM_SIZE);

#ifndef _FASTSORT_STACK
	free(random_array);
#endif
	return 0;

}


