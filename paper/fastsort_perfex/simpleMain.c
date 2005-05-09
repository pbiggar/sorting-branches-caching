#include <stdlib.h>
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
#error "You must define the sort function like this: \"gcc -DSEED=0\""
#endif

int main()
{
	int i;
	int random_array[RANDOM_SIZE];
	srand(SEED);
	for(i = 0; i < RANDOM_SIZE; i++)
	{
		random_array[i] = rand();
	}


	SORT(random_array, RANDOM_SIZE);

	return 0;
}


