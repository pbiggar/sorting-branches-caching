#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "cache_sorts.h"

#define LIMIT OUT_OF_PLACE_LIMIT
#define LIMIT_BITS OUT_OF_PLACE_LIMIT_BITS
static void
merge_flexible(unsigned int source[], int N, int starting_size, unsigned int target[]);
static void
merge(unsigned int source[], int N, int starting_size, unsigned int target[]);

static void
merge_reverse(unsigned int source[], int N, int starting_size, unsigned int target[]);
static void
merge_reverse_flexible(unsigned int source[], int N, int starting_size, unsigned int target[]);

static void
presort(unsigned int a[], int N);
static void
presort_flexible(unsigned int a[], int N);

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
double_tiled_mergesort(unsigned int a[], int N)
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


	/* get the address we need*/
	minusA = ((1 << BLOCK_BITS) - get_index(a));
	aux_data = memalign(ALIGNMENT, (N + 2*LIMIT) * sizeof(unsigned int));
	aux = (unsigned int*)(((unsigned int)aux_data & (~BLOCK_AND_LINE_MASK)) | (minusA << LINE_BITS));
	if (aux < aux_data) /* then the new index is less than the old one */
	{
		aux = (unsigned int*)((unsigned int)aux + (1 << (BLOCK_AND_LINE_BITS)));
	}

	if (N < 2048) /* fits in the level 1 cache */
	{
		presort_flexible(a, N);
		merge_flexible(a, N, 4, aux);
		goto end;
	}

/*	OUT(get_count(N)); */
/*	OUT(N); */

	level2_count = N / LIMIT; /* the number of standard LIMIT sized passes */
	level1_count = LIMIT / 2048; /* the number of standard 8k sized passes */
	extra_level2 = N % LIMIT; /* the number of extra items left, after the level 2 passes*/
	extra_level1_count = extra_level2 / 2048; /* number of extra level 1 passes  */
	final_extra = extra_level2 % 2048; /* number of items left over */

	level2_start = a;
	level2_aux_start = aux;

/*	printf("a = %p\n", a); */
/*	printf("aux = %p\n", aux); */

	for(i = 0; i < level2_count-1; i+=2) /* sort it level 2 */
	{
		unsigned int* level1_start = level2_start;
		unsigned int* level1_aux_start = level2_aux_start;
/*		printf("going into level2: i=%d\n", i); */
		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 2048);
			merge(level1_start, 2048, 4, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, 4, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge(level2_aux_start, LIMIT, 2048, level2_start);

		level2_start += LIMIT;
		level2_aux_start += LIMIT;


		/* now do it in reverse */

		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 2048);
			merge(level1_start, 2048, 4, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, 4, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge_reverse(level2_aux_start, LIMIT, 2048, level2_start);

		level2_start += LIMIT;
		level2_aux_start += LIMIT;

	}
	if (i < level2_count)
	{
		unsigned int* level1_start = level2_start;
		unsigned int* level1_aux_start = level2_aux_start;
/*		printf("going into level2: i=%d\n", i); */
		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 2048);
			merge(level1_start, 2048, 4, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, 4, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge(level2_aux_start, LIMIT, 2048, level2_start);

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
			presort(level1_start, 2048);
			merge(level1_start, 2048, 4, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, 4, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}
/*		OUT(extra_level1_single); */
		if (extra_level1_single)/* if there a full one left, its forward */
		{
			presort(level1_start, 2048);
			merge(level1_start, 2048, 4, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;
		}
/*		OUT(final_extra); */
		
		if (final_extra) /* theres less than a full level1 sized chunk */
		{
			presort_flexible(level1_start, final_extra);
			if (!extra_level1_single) /* should this be reversed or not */
				merge_flexible(level1_start, final_extra, 4, level1_aux_start);
			else merge_reverse_flexible(level1_start, final_extra, 4, level1_aux_start);

			/* find out where it ends, and copy it to the other if neccesary */
			if (!(get_count(final_extra) & 1)) /* its in aux, where it should be */
			{
				memcpy(level1_aux_start, level1_start, final_extra * sizeof(unsigned int));
			}
		}
		/* merge the whole extra into 1 */
		merge_flexible(level2_aux_start, extra_level2, 2048, level2_start);


		/* now put it into the right place */
		if (N > LIMIT)
		{
			if ((get_count(extra_level2) & 1) && (!(LIMIT_BITS & 1))) /* if its in aux and needs to be in a */
			{
/*				printf("copying %d ints from %p to %p\n", extra_level2, level2_start, level2_aux_start); */
				memcpy(level2_aux_start, level2_start, extra_level2 * sizeof(unsigned int));
			}
			else if ((!(get_count(extra_level2) & 1)) && (LIMIT_BITS & 1)) /* if its in a and needs to be in aux */
			{
/*				printf("copying %d ints from %p to %p\n", extra_level2, level2_aux_start, level2_start); */
				memcpy(level2_start, level2_aux_start, extra_level2 * sizeof(unsigned int));
			}
		}
	}
	
	/* now merge everything together */
	if (N > LIMIT)
	{
		if (LIMIT_BITS & 1) /* its in aux */
		{
			merge_flexible(aux, N, LIMIT, a);
		}
		else
		{
			merge_flexible(a, N, LIMIT, aux);
		}
	}

end:
	/* copy back if its in the wrong array */
	if (get_count(N) & 1)
	{
/*		printf("copying from %p to %p\n", aux, a); */
		memcpy(a, aux, N * sizeof(unsigned int)); 
	}

	free(aux_data);
}


static void
merge_flexible(unsigned int source[], int N, int starting_size, unsigned int target[])
{
	unsigned int *temp_pointer;
	unsigned int count = starting_size;
	unsigned int next_count = count * 2;

	int track = 0;
	int i = 0;
	int j = next_count - 1;
	int k = 0;
	int d = 1;

/*	printf("merge (%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
			
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];

			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				if (next_count >= N) /* this means we're done */
				{
					break;
				}

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

			j = i + next_count - 1;
			if (j >= N)
			{
				j = N-1;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = j;
			}
			else
			{
				d = 1;
				k = i;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i];
			i++;
			k += d;
		}
	}

/*	printf("%p\n", target); */
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

/*	printf("merge (%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
			
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i++;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j--;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];

			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				if (next_count >= N) /* this means we're done */
				{
					break;
				}

				/* the next iteration will be oevr bigger lists */
				count = next_count;
				next_count <<= 1;


				temp_pointer = source;
				source = target;
				target = temp_pointer;

				i = 0;
				j = next_count - 1;
				track = 0;
				k = 0;
				d = 1;
				continue;
			}

			j = i + next_count - 1;

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = j;
			}
			else
			{
				d = 1;
				k = i;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i];
			i++;
			k += d;
		}
	}

/*	printf("%p\n", target); */
}

static void
merge_reverse_flexible(unsigned int source[], int N, int starting_size, unsigned int target[])
{
	unsigned int *temp_pointer;
	unsigned int count = starting_size;
	unsigned int next_count = count * 2;

	int track = N-1;
	int i = N-1;
	int j = i - (next_count - 1);
	int k = i;
	int d = -1;

/*	printf("merge reverse(%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
			
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
		
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];

			i = track - next_count;

			/* check whether we have left the building */
			if (i <= 0)
			{
				if (next_count >= N) /* this means we're done */
				{
					break;
				}

				/* the next iteration will be oevr bigger lists */
				count = next_count;
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

			j = i - (next_count - 1);
			if (j <= 0)
			{
				j = 0;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = i;
			}
			else
			{
				d = 1;
				k = j;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i];
			i--;
			k += d;
		}
	}
/*	printf("%p\n", target); */
}
static void
merge_reverse(unsigned int source[], int N, int starting_size, unsigned int target[])
{
	unsigned int *temp_pointer;
	unsigned int count = starting_size;
	unsigned int next_count = count * 2;

	int track = N-1;
	int i = N-1;
	int j = i - (next_count - 1);
	int k = i;
	int d = -1;

/*	printf("merge reverse(%p, %d, %d, %p) => ", source, N, starting_size, target); */

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
			
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;

			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
		
			if (source[i] >= source[j])
				break;
			target[k] = source[i];
			k += d;
			i--;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;

			if (source[i] <= source[j])
				break;
			target[k] = source[j];
			k += d;
			j++;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];

			i = track - next_count;

			/* check whether we have left the building */
			if (i <= 0)
			{
				if (next_count >= N) /* this means we're done */
				{
					break;
				}

				/* the next iteration will be oevr bigger lists */
				count = next_count;
				next_count <<= 1;

				temp_pointer = source;
				source = target;
				target = temp_pointer;

				i = N-1;
				j = i - (next_count - 1);
				track = i;
				k = i;
				d = -1;
				continue;
			}

			j = i - (next_count - 1);

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = i;
			}
			else
			{
				d = 1;
				k = j;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i];
			i--;
			k += d;
		}
	}
/*	printf("%p\n", target); */
}

static void
presort(unsigned int a[], int N)
{
	int i;
	int j;

	for(i = 0; i < N; i+=8)
	{
		j = i+1;
		if (less(a[i+2], a[j])) j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i], a[j]);

		j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i+1], a[j]);

		compexch(a[i+2], a[i+3]);

		j = i+5;

		if (less(a[j], a[i+6])) j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+4]);

		j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+5]);

		compexch(a[i+7], a[i+6]);
	}
}

static void
presort_flexible(unsigned int a[], int N)
{
	int i;
	int j;
	int temp;
	int min;

	for(i = 0; i <= N-8; i+=8)
	{
		j = i+1;
		if (less(a[i+2], a[j])) j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i], a[j]);

		j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i+1], a[j]);

		compexch(a[i+2], a[i+3]);

		j = i+5;

		if (less(a[j], a[i+6])) j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+4]);

		j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+5]);

		compexch(a[i+7], a[i+6]);
	}

	/* do a selection sort for the next 4, if they exist */
	temp = i+4;
	if (N < temp) temp = N;
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

