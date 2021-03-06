#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "cache_sorts.h"

#define LIMIT OUT_OF_PLACE_LIMIT
#define LIMIT_BITS OUT_OF_PLACE_LIMIT_BITS

#define ODD_COUNT 8
#define EVEN_COUNT 4

static void
merge(unsigned int source[], int N, int starting_size, unsigned int target[]);

static void
merge_reverse(unsigned int source[], int N, int starting_size, unsigned int target[]);

static void
presort(unsigned int a[], int N);
static void
presort_flexible(unsigned int a[], int N);
static void
presort_flexible_reverse(unsigned int a[], int N);

static int presort_count = 8;
static int double_presort_count = 16;

inline static void set_presort_count(int count)
{
	presort_count = count;
	double_presort_count = count * 2;
}
/*
static void
check_merge_reverse(unsigned int a[], int N)
{
	int i;
	for(i = 0; i < N-1; i++)
	{
		if (a[i+1] > a[i])
		{
			printf("bad reverse: A: %p, a[%u] = %u, a[%u] = %u\n", a, i, a[i], i+1, a[i+1]);
			return;
		}
	}
}
static void
check_merge(unsigned int a[], int N)
{
	int i;
	for(i = 0; i < N-1; i++)
	{
		if (a[i] > a[i+1])
		{
			printf("bad forwards: A: %p, a[%u] = %u, a[%u] = %u\n", a, i, a[i], i+1, a[i+1]);
			return;
		}
	}
}
*/

static int get_count(const int N)
{

	int lg2N = 32; /* im hard coding this and i dont care */
	unsigned int msbN = 1 << (lg2N - 1);
	while(msbN >= N)
	{
		lg2N--;
		msbN = msbN >> 1;
	}
	return lg2N;
}

void
tiled_mergesort(unsigned int a[], int N)
{
	/* track keeps an eye on where i started last */
	/* outer track keeps and eye on which cache_sized/2 segemnt i started in last */
	int i,j; /* indices for the first array */
	
	int level1_count;
	int level2_count;
	int extra_level1_count = 0;
	int extra_level2;
	int final_extra;


	unsigned int* aux_data;
	unsigned int* aux;

	unsigned int* level2_start;
	unsigned int* level2_aux_start;

	unsigned int minusA;

	unsigned int** level1_finish;
	unsigned int** level1_other;

	int odd = 0;

	describe_predictor(&global_predictor[0], "forwards middle");
	describe_predictor(&global_predictor[1], "forwards next");
	describe_predictor(&global_predictor[2], "forwards end");
	describe_predictor(&global_predictor[3], "forwards setup");
	describe_predictor(&global_predictor[4], "forwards equal");
	describe_predictor(&global_predictor[5], "reverse middle");
	describe_predictor(&global_predictor[6], "reverse next");
	describe_predictor(&global_predictor[7], "reverse end");
	describe_predictor(&global_predictor[8], "reverse setup");
	describe_predictor(&global_predictor[9], "reverse equal");
	describe_predictor(&global_predictor[10], "insertion outer");
	describe_predictor(&global_predictor[11], "insertion inner");
	describe_predictor(&global_predictor[12], "insertion reverse outer");
	describe_predictor(&global_predictor[13], "insertion reverse inner");
#ifdef _USE_ROLLED_LOOPS

	describe_predictor(&global_predictor[14], "forwards left");
	describe_predictor(&global_predictor[15], "forwards right");
	describe_predictor(&global_predictor[16], "reverse left");
	describe_predictor(&global_predictor[17], "reverse right");
#else
	describe_predictor(&global_predictor[14], "forwards left 0");
	describe_predictor(&global_predictor[15], "forwards left 1");
	describe_predictor(&global_predictor[16], "forwards left 2");
	describe_predictor(&global_predictor[17], "forwards left 3");
	describe_predictor(&global_predictor[18], "forwards left 4");
	describe_predictor(&global_predictor[19], "forwards left 5");
	describe_predictor(&global_predictor[20], "forwards left 6");
	describe_predictor(&global_predictor[21], "forwards left 7");
	describe_predictor(&global_predictor[22], "forwards right 0");
	describe_predictor(&global_predictor[23], "forwards right 1");
	describe_predictor(&global_predictor[24], "forwards right 2");
	describe_predictor(&global_predictor[25], "forwards right 3");
	describe_predictor(&global_predictor[26], "forwards right 4");
	describe_predictor(&global_predictor[27], "forwards right 5");
	describe_predictor(&global_predictor[28], "forwards right 6");
	describe_predictor(&global_predictor[29], "forwards right 7");

	describe_predictor(&global_predictor[30], "reverse left 0");
	describe_predictor(&global_predictor[31], "reverse left 1");
	describe_predictor(&global_predictor[32], "reverse left 2");
	describe_predictor(&global_predictor[33], "reverse left 3");
	describe_predictor(&global_predictor[34], "reverse left 4");
	describe_predictor(&global_predictor[35], "reverse left 5");
	describe_predictor(&global_predictor[36], "reverse left 6");
	describe_predictor(&global_predictor[37], "reverse left 7");
	describe_predictor(&global_predictor[38], "reverse right 0");
	describe_predictor(&global_predictor[39], "reverse right 1");
	describe_predictor(&global_predictor[40], "reverse right 2");
	describe_predictor(&global_predictor[41], "reverse right 3");
	describe_predictor(&global_predictor[42], "reverse right 4");
	describe_predictor(&global_predictor[43], "reverse right 5");
	describe_predictor(&global_predictor[44], "reverse right 6");
	describe_predictor(&global_predictor[45], "reverse right 7");
#endif

	/* a quick explanation, cause I keep needing to be reminded how I did this:
	 * an address is split into 3 parts: the tag, the index and the offset.
	 * Suppose theres a 32 bit address, a 32 byte cache line and 65536 cache
	 * blocks, as in our tests. In this case, the 32 bit address is split into
	 * a 5 bit offset (2^5 = 32 byte cache line), a 16 bit index (2^16 = 65536
	 * cache blocks) and the rest is the tag.  Therefore, a and aux need to
	 * have exactly the opposite index. minusA is the index aux needs to have,
	 * which we mask in. If this results in an address lower than the one we
	 * started with (in aux), then thats out of bounds, and increase it by
	 * 65536.
	 */

	/* get the index we need*/
	minusA = get_index(a) ^ (1 << (BLOCK_BITS - 1));

	aux_data = memalign(ALIGNMENT, (N + 2*LIMIT) * sizeof(unsigned int));

	/* clear the index bits, and mask in the desired index */
	aux = (unsigned int*)(((unsigned int)aux_data & (~BLOCK_AND_LINE_MASK)) | (minusA << LINE_BITS));
	if (aux < aux_data) /* then the new index is less than the old one */
	{
		aux = (unsigned int*)((unsigned int)aux + (1 << (BLOCK_AND_LINE_BITS)));
	}



	if (N <= 2048) /* fits in the level 1 cache */
	{
		if (get_count(N) & 1) set_presort_count(ODD_COUNT);
		else set_presort_count(EVEN_COUNT);
			
		presort_flexible(a, N);
		merge(a, N, presort_count, aux);
		goto end;
	}

/*	OUT(get_count(N)); */
/*	OUT(N); */

	level2_count = N / LIMIT; /* the number of standard LIMIT sized passes */
	level1_count = LIMIT / 1024; /* the number of standard 4k sized passes, per level 2 iteration */
	extra_level2 = N % LIMIT; /* the number of extra items left, after the level 2 passes*/
	extra_level1_count = extra_level2 / 1024; /* number of extra level 1 passes  */
	final_extra = extra_level2 % 1024; /* number of items left over */

	level2_start = a;
	level2_aux_start = aux;

	/*make sure it ends up in a, not aux */
	/* odd means it should end up in aux. and the final merge will get it into a */
	/* even means it should end up in a, and the final mergre will do an even number of steps */
	if (get_count(N) & 1)
	{
		level1_finish =  &level2_aux_start;
		level1_other = &level2_start;
		set_presort_count(ODD_COUNT);
		odd = 1;
	}
	else
	{
		level1_finish =  &level2_start;
		level1_other = &level2_aux_start;
		set_presort_count(EVEN_COUNT);
		odd = 0;
	}

/*	printf("a = %p\n", a); 
	printf("aux = %p\n", aux); */

	for(i = 0; i < level2_count-1; i+=2) /* sort it level 2 */
	{
		unsigned int* level1_start = level2_start;
		unsigned int* level1_aux_start = level2_aux_start;
/*		printf("going into level2: i=%d\n", i); */
		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 1024);
			merge(level1_start, 1024, presort_count, level1_aux_start);
			level1_start += 1024;
			level1_aux_start += 1024;

			/* now reverse it */

			presort(level1_start, 1024);
			merge_reverse(level1_start, 1024, presort_count, level1_aux_start);
			level1_start += 1024;
			level1_aux_start += 1024;
		}

		/* merge them all into LIMIT sized bits */
		merge(*level1_finish, LIMIT, 1024, *level1_other);

		level2_start += LIMIT;
		level2_aux_start += LIMIT;


		/* now do it in reverse */

		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 1024);
			merge(level1_start, 1024, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 1024;
			level1_aux_start += 1024;

			/* now reverse it */

			presort(level1_start, 1024);
			merge_reverse(level1_start, 1024, presort_count, level1_aux_start);
			level1_start += 1024;
			level1_aux_start += 1024;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge_reverse(*level1_finish, LIMIT, 1024, *level1_other);

		level2_start += LIMIT;
		level2_aux_start += LIMIT;

	}
	if (i < level2_count)
	{
		unsigned int* level1_start = level2_start;
		unsigned int* level1_aux_start = level2_aux_start;
		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 1024);
			merge(level1_start, 1024, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 1024;
			level1_aux_start += 1024;

			/* now reverse it */

			presort(level1_start, 1024);
			merge_reverse(level1_start, 1024, presort_count, level1_aux_start);
			level1_start += 1024;
			level1_aux_start += 1024;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge(*level1_finish, LIMIT, 1024, *level1_other);

		level2_start += LIMIT;
		level2_aux_start += LIMIT;
	}

	/* sort the remaining bits */
	/* level2 start is in the right place */
/*	OUT(extra_level1_count); */
	if (extra_level2) /* there is a maximum of 1 extra tevel 2 sort */
	{
		int extra_level1_single = extra_level1_count & 0x1;
		unsigned int* level1_start = level2_start;
		unsigned int* level1_aux_start = level2_aux_start;
		extra_level1_count &= ~0x1; /*clear the last bit */
/*		OUT(extra_level2); */
/*		OUT(extra_level1_count); */
		for(j = 0; j < extra_level1_count; j+=2) /* merge the level 1 cache first */
		{
			presort(level1_start, 1024);
			merge(level1_start, 1024, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 1024;
			level1_aux_start += 1024;

			/* now reverse it */

			presort(level1_start, 1024);
			merge_reverse(level1_start, 1024, presort_count, level1_aux_start);
			level1_start += 1024;
			level1_aux_start += 1024;
			/* these end up in aux */
		}
/*		OUT(extra_level1_single); */
		if (extra_level1_single)/* if there a full one left, its forward */
		{
			presort(level1_start, 1024);
			merge(level1_start, 1024, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 1024;
			level1_aux_start += 1024;
		}
/*		OUT(final_extra); */

		if (final_extra) /* theres less than a full level1 sized chunk */
		{
			/* these will be sorted in one go */
			/* if it turns out the number should be 2049, I may need to change this */
			if (final_extra <= double_presort_count)
			{
				if (!odd) set_presort_count(ODD_COUNT);
			}
			else
			{
				if (get_count(final_extra) & 1)
				{
					if (odd) set_presort_count(EVEN_COUNT);
					else set_presort_count(ODD_COUNT);
				}
			}

			/* should this be reversed or not */
			if (!extra_level1_single) 
			{
				presort_flexible(level1_start, final_extra);
				merge(level1_start, final_extra, presort_count, level1_aux_start);
			}
			else 
			{
				presort_flexible_reverse(level1_start, final_extra);
				merge_reverse(level1_start, final_extra, presort_count, level1_aux_start);
			}
		}

		/* merge the whole extra into 1 */
		merge(*level1_finish, extra_level2, 1024, *level1_other);
	}
	
	/* now merge everything together */
	if (N > LIMIT)
	{
		if (odd) /* its in aux - we took steps to ensure it*/
		{
			merge(aux, N, LIMIT, a);
		}
		else
		{
			merge(a, N, LIMIT, aux);
		}
	}

end:

	free(aux_data);
	init_predictor(&global_predictor[0]);
	init_predictor(&global_predictor[1]);
	init_predictor(&global_predictor[2]);
	init_predictor(&global_predictor[3]);
	init_predictor(&global_predictor[4]);
	init_predictor(&global_predictor[5]);
	init_predictor(&global_predictor[6]);
	init_predictor(&global_predictor[7]);
	init_predictor(&global_predictor[8]);
	init_predictor(&global_predictor[9]);
	init_predictor(&global_predictor[10]);
	init_predictor(&global_predictor[11]);
	init_predictor(&global_predictor[12]);
	init_predictor(&global_predictor[13]);
}


static void
merge(unsigned int source[], int N, int starting_size, unsigned int target[])
{
	unsigned int *temp_pointer;
	unsigned int count = starting_size;
	unsigned int next_count = count * 2;

	int track = 0;
	int i = 0;
	int j = next_count - 1;
	int k = 0;
	int d = 1;
	if (j >= N) j = N-1;

/*	printf("merge (%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[14]);
				break;
			}
			branch_not_taken(&global_predictor[14]);
			target[k] = source[i];
			k += d;
			i++;

#ifndef _USE_ROLLED_LOOPS
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[15]);
				break;
			}
			branch_not_taken(&global_predictor[15]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[16]);
				break;
			}
			branch_not_taken(&global_predictor[16]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[17]);
				break;
			}
			branch_not_taken(&global_predictor[17]);
			target[k] = source[i];
			k += d;
			i++;
			
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[18]);
				break;
			}
			branch_not_taken(&global_predictor[18]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[19]);
				break;
			}
			branch_not_taken(&global_predictor[19]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[20]);
				break;
			}
			branch_not_taken(&global_predictor[20]);
			target[k] = source[i];
			k += d;
			i++;
		
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[21]);
				break;
			}
			branch_not_taken(&global_predictor[21]);
			target[k] = source[i];
			k += d;
			i++;
#endif
		}

		/* iterate through the right list */
		while(1)
		{
#ifdef _USE_ROLLED_LOOPS
			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[15]);
				break;
			}
			branch_not_taken(&global_predictor[15]);
			target[k] = source[j];
			k += d;
			j--;
#else
			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[22]);
				break;
			}
			branch_not_taken(&global_predictor[22]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[23]);
				break;
			}
			branch_not_taken(&global_predictor[23]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[24]);
				break;
			}
			branch_not_taken(&global_predictor[24]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[25]);
				break;
			}
			branch_not_taken(&global_predictor[25]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[26]);
				break;
			}
			branch_not_taken(&global_predictor[26]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[27]);
				break;
			}
			branch_not_taken(&global_predictor[27]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[28]);
				break;
			}
			branch_not_taken(&global_predictor[28]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[29]);
				break;
			}
			branch_not_taken(&global_predictor[29]);
			target[k] = source[j];
			k += d;
			j--;
#endif
		}

		/* check if we're in the middle */
		if(i == j)
		{
			branch_taken(&global_predictor[0]);
			target[k] = source[i];

			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				branch_taken(&global_predictor[1]);
				if (next_count > N) /* this means we're done */
				{
					branch_taken(&global_predictor[2]);
					break;
				}
				branch_not_taken(&global_predictor[2]);

				/* the next iteration will be oevr bigger lists */
				count = next_count;
				next_count <<= 1;


				temp_pointer = source;
				source = target;
				target = temp_pointer;

				i = 0;
				j = next_count - 1;
				if (j >= N)
				{
					j = N-1;
				}
				track = 0;
				k = 0;
				d = 1;
				continue;
			}
			else branch_not_taken(&global_predictor[1]);

			j = i + next_count - 1;
			if (j >= N)
			{
				j = N-1;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				branch_taken(&global_predictor[3]);
				d = -1;
				k = j;
			}
			else
			{
				branch_not_taken(&global_predictor[3]);
				d = 1;
				k = i;
			}
		}
		else 
		{
			branch_not_taken(&global_predictor[0]);
			if (source[i] == source[j])
			{
				branch_taken(&global_predictor[4]);
				target[k] = source[i];
				i++;
				k += d;
			}
			else branch_not_taken(&global_predictor[4]);
		}
	}

/*	printf("%p\n", target); */
}

static void
merge_reverse(unsigned int source[], int N, int starting_size, unsigned int target[])
{
	unsigned int *temp_pointer;
	unsigned int next_count = starting_size * 2;

	int track = N-1;
	int i = N-1;
	int j = i - (next_count - 1);
	int k = i;
	int d = -1;
	if (j < 0) j = 0;

/*	printf("merge reverse(%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
#ifdef _USE_ROLLED_LOOPS
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[16]);
				break;
			}
			branch_not_taken(&global_predictor[16]);
			target[k] = source[i];
			k += d;
			i--;
#else
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[30]);
				break;
			}
			branch_not_taken(&global_predictor[30]);
			target[k] = source[i];
			k += d;
			i--;


			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[31]);
				break;
			}
			branch_not_taken(&global_predictor[31]);
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[32]);
				break;
			}
			branch_not_taken(&global_predictor[32]);
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[33]);
				break;
			}
			branch_not_taken(&global_predictor[33]);
			target[k] = source[i];
			k += d;
			i--;
			
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[34]);
				break;
			}
			branch_not_taken(&global_predictor[34]);
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[35]);
				break;
			}
			branch_not_taken(&global_predictor[35]);
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[36]);
				break;
			}
			branch_not_taken(&global_predictor[36]);
			target[k] = source[i];
			k += d;
			i--;
		
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[37]);
				break;
			}
			branch_not_taken(&global_predictor[37]);
			target[k] = source[i];
			k += d;
			i--;
#endif
		}

		/* iterate through the right list */
		while(1)
		{

#ifdef _USE_ROLLED_LOOPS
			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[17]);
				break;
			}
			branch_not_taken(&global_predictor[17]);
			target[k] = source[j];
			k += d;
			j++;
#else
			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[38]);
				break;
			}
			branch_not_taken(&global_predictor[38]);
			target[k] = source[j];
			k += d;
			j++;


			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[39]);
				break;
			}
			branch_not_taken(&global_predictor[39]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[40]);
				break;
			}
			branch_not_taken(&global_predictor[40]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[41]);
				break;
			}
			branch_not_taken(&global_predictor[41]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[42]);
				break;
			}
			branch_not_taken(&global_predictor[42]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[43]);
				break;
			}
			branch_not_taken(&global_predictor[43]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[44]);
				break;
			}
			branch_not_taken(&global_predictor[44]);
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[45]);
				break;
			}
			branch_not_taken(&global_predictor[45]);
			target[k] = source[j];
			k += d;
			j++;

#endif
		}

		/* check if we're in the middle */
		if(i == j)
		{
			branch_taken(&global_predictor[5]);
			target[k] = source[i];

			i = track - next_count;

			/* check whether we have left the building */
			if (i < 0)
			{
				branch_taken(&global_predictor[6]);
				if (next_count > N) /* this means we're done */
				{
					branch_taken(&global_predictor[7]);
					break;
				}
				branch_not_taken(&global_predictor[7]);

				/* the next iteration will be oevr bigger lists */
				next_count <<= 1;

				temp_pointer = source;
				source = target;
				target = temp_pointer;

				i = N-1;
				j = i - (next_count - 1);
				if (j <= 0)
				{
					j = 0;
				}
				track = i;
				k = i;
				d = -1;
				continue;
			}
			else branch_not_taken(&global_predictor[6]);

			j = i - (next_count - 1);
			if (j <= 0)
			{
				j = 0;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				branch_taken(&global_predictor[8]);
				d = -1;
				k = i;
			}
			else
			{
				branch_not_taken(&global_predictor[8]);
				d = 1;
				k = j;
			}
		}
		else 
		{
			branch_not_taken(&global_predictor[5]);
			
			if (source[i] == source[j])
			{
				branch_taken(&global_predictor[9]);
				target[k] = source[i];
				i--;
				k += d;
			}
			else branch_not_taken(&global_predictor[9]);
		}
	}
/*	printf("%p\n", target); */
}


static void
insertion(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = 1; i < N; i++)
	{
		int j = i;
		branch_taken(&global_predictor[10]);
		v = a[i];

		while((j > 0) && less(v, a[j-1]))
		{
			branch_taken(&global_predictor[11]);
			a[j] = a[j-1];
			j--;
		}
		branch_not_taken(&global_predictor[11]);
		a[j] = v;
	}
	branch_not_taken(&global_predictor[10]);
}
static void
insertion_reverse(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = N-2; i >= 0; i--)
	{
		int j = i;
		branch_taken(&global_predictor[12]);
		v = a[i];

		while((j < N-1) && less(v, a[j+1]))
		{
			branch_taken(&global_predictor[13]);
			a[j] = a[j+1];
			j++;
		}
		branch_not_taken(&global_predictor[13]);
		a[j] = v;
	}
	branch_not_taken(&global_predictor[12]);
}

static void
presort(unsigned int a[], int N)
{
	int i;
	for(i = 0; i < N; i+=double_presort_count)
	{
		insertion(&a[i], presort_count);
		insertion_reverse(&a[i+presort_count], presort_count);
	}
}

static void
presort_flexible(unsigned int a[], int N)
{
	int i;
	int j;
	int temp;
	int min;

	for(i = 0; i < N-double_presort_count; i+=double_presort_count)
	{
		insertion(&a[i], presort_count);
		insertion_reverse(&a[i+presort_count], presort_count);
	}
	/* do a selection sort for the next 4, if they exist */
	temp = i+presort_count;
	if (N <= temp) temp = N;
	for(; i < temp-1; i++)
	{
		min = i;
		for(j = i+1; j < temp; j++)
		{
			if(less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}

	/* do a reverse sort for the remaining ones, if they exist */
	for(i = temp; i < N-1; i++)
	{
		min = i;
		for(j = i+1; j < N; j++)
		{
			if(!less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}
}

static void
presort_flexible_reverse(unsigned int a[], int N)
{
	/* theres no downside to doing this in reverse. we may go over cache block
	 * boundaries, but they need to be loaded anyway, and this is only done on
	 * arrays that fit in the level 1 cache
	 */
	int i;
	int j;
	int temp;
	int min;
	int extra = N % double_presort_count;

	for(i = extra; i < N; i+=double_presort_count)
	{
		insertion(&a[i], presort_count);
		insertion_reverse(&a[i+presort_count], presort_count);
	}

	/* do a selection sort for the next 4, if there are any */
	temp = extra - presort_count;
	if (temp <= 0) temp = 0;
	for(i = 0; i < temp-1; i++)
	{
		min = i;
		for(j = i+1; j < temp; j++)
		{
			if(less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}
	/* do a reverse sort for the remaining ones, if which there should be*/
	for(i = temp; i < extra-1; i++)
	{
		min = i;
		for(j = i+1; j < extra; j++)
		{
			if(!less(a[j],a[min]))
			{
				min = j;
			}
		}
		exch(a[i], a[min]);
	}




}
