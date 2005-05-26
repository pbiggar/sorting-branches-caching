#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <limits.h>

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

static int presort_count = 8;
static int double_presort_count = 16;

static void
fix_up(unsigned int heap[], int child)
{
	int parent;

	unsigned int v = heap[child];
	while(less(v, heap[parent = ((child-1)/HEAP_SIZE)]))
	{
		heap[child] = heap[parent];
		heap[parent] = v;
		child = parent;
	}
}

static void
fix_down(unsigned int heap[], int parent, int N)
{
	int child;
	int child2;
	unsigned int v = heap[parent];

	while(parent*HEAP_SIZE+1 < N) /* see if it has any children */
	{
		child = parent*HEAP_SIZE + 1; /* this gets the rightmost node */
		child2 = child;

		if (less(heap[child2 + 0], heap[child])) child = child2 + 0;
		if (less(heap[child2 + 1], heap[child])) child = child2 + 1;
		if (less(heap[child2 + 2], heap[child])) child = child2 + 2;
		if (less(heap[child2 + 3], heap[child])) child = child2 + 3;

#if (HEAP_SIZE == 8)
		if (less(heap[child2 + 4], heap[child])) child = child2 + 4;
		if (less(heap[child2 + 5], heap[child])) child = child2 + 5;
		if (less(heap[child2 + 6], heap[child])) child = child2 + 6;
		if (less(heap[child2 + 7], heap[child])) child = child2 + 7;

#endif

		/* when the larger child isnt large enough to be promoted, stop */
		if (!less(heap[child], v)) break;

		/* move down */
		heap[parent] = heap[child];
		parent = child;
	}
	heap[parent] = v;
}

inline static void
add_heap_line(unsigned int a[], unsigned int heap[], int direction, int length, int c)
{
	int j;
	for(j = 0; j < c; j++) /*we do the last one ouselves */
	{
		heap[length] = a[(j * direction)];
		fix_up(heap, length);
		length++;
	}
}

inline static void
sort_last_index(unsigned int a[], unsigned int aux[], int N)
{
	int i;
	unsigned int v, v2;
	for(i = 1; i < N; i++)
	{
		int j = i;
		v = a[i];
		v2 = aux[i];

		while((j > 0) && less(v, a[j-1]))
		{
			a[j] = a[j-1];
			aux[j] = aux[j-1];
			j--;
		}
		a[j] = v;
		aux[j] = v2; /* a[i] doesnt change at all */
	}
}
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
			printf("bad reverse: A: %p, a[%d] = %d, a[%d] = %d\n", a, i, a[i], i+1, a[i+1]);
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
			printf("bad forwards: A: %p, a[%d] = %d, a[%d] = %d\n", a, i, a[i], i+1, a[i+1]);
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
double_multi_mergesort(unsigned int a[], int N)
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

	unsigned int *heap_data, *heap, *last_added;
	unsigned int *indices, *last_added_indices, *last_added_data;

	int odd = 0;
	int K;


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
		if (get_count(N) & 1) set_presort_count(ODD_COUNT);
		else set_presort_count(EVEN_COUNT);
			
		presort_flexible(a, N);
		merge(a, N, presort_count, aux);
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

	/*make sure it ends up in a, not aux */
	/* odd means it should end up in aux. and the final merge will get it into a */
	/* even means it should end up in a, and the final mergre will do an even number of steps */
	if (get_count(N) & 1)
	{
		level1_finish =  &level2_start;
		level1_other = &level2_aux_start;
		set_presort_count(N < LIMIT ? ODD_COUNT : EVEN_COUNT);
		odd = 1;
	}
	else
	{
		level1_finish =  &level2_aux_start;
		level1_other = &level2_start;
		set_presort_count(N < LIMIT ? EVEN_COUNT : ODD_COUNT);
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
			presort(level1_start, 2048);
			merge(level1_start, 2048, presort_count, level1_aux_start);
/*			check_merge(level1_start, 2048);*/
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, presort_count, level1_aux_start);
/*			check_merge_reverse(level1_start, 2048);*/
			level1_start += 2048;
			level1_aux_start += 2048;
		}

		/* merge them all into LIMIT sized bits */
		merge(*level1_finish, LIMIT, 2048, *level1_other);
/*		check_merge(*level1_finish, LIMIT);*/

		level2_start += LIMIT;
		level2_aux_start += LIMIT;


		/* now do it in reverse */

		for(j = 0; j < level1_count; j+=2) /* merge the level 1 cache first */
		{
/*			printf("going into level1: j=%d\n", j); */
			presort(level1_start, 2048);
			merge(level1_start, 2048, presort_count, level1_aux_start); /* after this they end up in aux */
/*			check_merge(level1_start, 2048);*/
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, presort_count, level1_aux_start);
/*			check_merge_reverse(level1_start, 2048);*/
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge_reverse(*level1_finish, LIMIT, 2048, *level1_other);
/*		check_merge_reverse(*level1_finish, LIMIT);*/

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
			merge(level1_start, 2048, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, presort_count, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}

		/* merge them all into LIMIT sized bits */
		merge(*level1_finish, LIMIT, 2048, *level1_other);

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
			merge(level1_start, 2048, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;

			/* now reverse it */

			presort(level1_start, 2048);
			merge_reverse(level1_start, 2048, presort_count, level1_aux_start);
			level1_start += 2048;
			level1_aux_start += 2048;
			/* these end up in aux */
		}
/*		OUT(extra_level1_single); */
		if (extra_level1_single)/* if there a full one left, its forward */
		{
			presort(level1_start, 2048);
			merge(level1_start, 2048, presort_count, level1_aux_start); /* after this they end up in aux */
			level1_start += 2048;
			level1_aux_start += 2048;
		}
/*		OUT(final_extra); */

		if (final_extra) /* theres less than a full level1 sized chunk */
		{
			if ((get_count(final_extra) & 1) ^ odd) /* an exclusive or i hope, if either, but not both, then somethings wrong */
			{
				if (odd) set_presort_count(ODD_COUNT);
				else set_presort_count(EVEN_COUNT);
			}

			presort_flexible(level1_start, final_extra);

			if (!extra_level1_single) /* should this be reversed or not */
				merge(level1_start, final_extra, presort_count, level1_aux_start);
			else merge_reverse(level1_start, final_extra, presort_count, level1_aux_start);

		}

		/* merge the whole extra into 1 */
		merge(*level1_finish, extra_level2, 2048, *level1_other);
	}
	
/*#define K (((N-1) >> (LIMIT_BITS))+1)*/
	K = (((N-1) >> (LIMIT_BITS))+1);
/*	K = level2_count;*/
/*	if (extra_level2) level2_count += 1;*/
	/* now merge everything together */
	if (N > LIMIT) /* else its already sorted */
	{
		int heap_length = 0;
		/* check everything merged OK*/
		indices = memalign(HEAP_SIZE, ((K+1)^1) * sizeof(int)); /* this changes to an even number (rounding up) */
		heap_data = memalign(HEAP_SIZE, (K+1) * HEAP_SIZE * sizeof(unsigned int));
		heap = heap_data + (HEAP_SIZE - 1);
		memset(heap_data, 0xff, (K+1) * HEAP_SIZE * sizeof(unsigned int));
		last_added_data = memalign(HEAP_SIZE, (K+1) * sizeof(unsigned int));
		last_added_indices = memalign(HEAP_SIZE, (K) * sizeof(unsigned int));

		last_added = &last_added_data[1];
		last_added_data[0] = 0;


		/* we add 8 from each to the Q */
		for(i = 0; i < K; i++) /* we do the last one ourselves */
		{
			/* this may cause k misses in the bimodal predictor. but k is so small thats its not worth the effort */
			if (i & 1) /* this is a reverse list */
			{
				if (i == K-1) indices[i] = N-1; /* last mini-list */
				else indices[i] = ((i+1) << LIMIT_BITS) - 1;

				add_heap_line(&aux[indices[i]], heap, -1, heap_length, HEAP_SIZE);
				indices[i] -= HEAP_SIZE;

				last_added[i] = aux[indices[i]+1];
				last_added_indices[i] = i;

			}
			else /* foward list */
			{
				indices[i] = i << LIMIT_BITS;
				if (i == K-1)
				{
					int space_to_end = HEAP_SIZE;
					if (N - indices[i] < HEAP_SIZE ) space_to_end = N - indices[i];

					indices[i+1] = N-1; /* this makes a sort of sentinel ,which simplifies the logic below */
					add_heap_line(&aux[indices[i]], heap, 1, heap_length, space_to_end); /* FIXME if there arent 8 in the last list this could be awkward */
					indices[i] += space_to_end;
				}
				else
				{
					add_heap_line(&aux[indices[i]], heap, 1, heap_length, HEAP_SIZE);
					indices[i] += HEAP_SIZE;
				}

				last_added[i] = aux[indices[i]-1];
				last_added_indices[i] = i;
			}
			heap_length += HEAP_SIZE;
		}
/*		print_int_array(indices, K, "indices", -1, -1, -1);
		print_int_array(last_added_indices, K, "last added indices", -1, -1, -1);
		print_int_array(last_added, K, "last added ", -1, -1, -1);*/

		sort_last_index(last_added, last_added_indices, K);
/*		print_int_array(last_added_indices, K, "adter sorting indices", -1, -1, -1);
		print_int_array(last_added, K, "adter sorting ", -1, -1, -1); */

		/* do the merge */
		i = 0;
		while(i < N)
		{
			a[i] = heap[0];
			heap[0] = heap[heap_length-1];
			heap[heap_length-1] = UINT_MAX;
			fix_down(heap, 0, --heap_length);

			if (a[i] == last_added[0])
			{
				int last_added_index = last_added_indices[0];
/*				if (heap_length <= K) printf("adding more: length=%d, i = %d\n", heap_length, i);*/
					
				if (last_added_index & 1)
				{
					int space_to_end = indices[last_added_index] - indices[last_added_index - 1] + 1; /* we add 1 here because both indices are on an item that hasnt yet been added */
					if (space_to_end < HEAP_SIZE) /* bounds check (l_a_i must have a buddy) */
					{
/*						printf("bounds check is happening\n");*/
						add_heap_line(&aux[indices[last_added_index]], heap, -1, heap_length, space_to_end);
						indices[last_added_index] -= space_to_end;
						last_added[0] = UINT_MAX;
						heap_length += space_to_end;
					}
					else
					{
						add_heap_line(&aux[indices[last_added_index]], heap, -1, heap_length, HEAP_SIZE);
						indices[last_added_index] -= HEAP_SIZE;
						last_added[0] = aux[indices[last_added_index]+1];
						heap_length += HEAP_SIZE;
					}
				}
				else
				{
					int space_to_end = indices[last_added_index+1] - indices[last_added_index] + 1;
					/* the will have a buddy on the right */
					if (space_to_end < HEAP_SIZE)
					{
/*						printf("bounds check is happening\n");*/
						add_heap_line(&aux[indices[last_added_index]], heap, 1, heap_length, space_to_end);
						indices[last_added_index] += space_to_end;
						last_added[0] = UINT_MAX;
						heap_length += space_to_end;
					}
					else
					{
						add_heap_line(&aux[indices[last_added_index]], heap, 1, heap_length, HEAP_SIZE);
						indices[last_added_index] += HEAP_SIZE;
						last_added[0] = aux[indices[last_added_index]-1];
						heap_length += HEAP_SIZE;
					}
				}

				sort_last_index(last_added, last_added_indices, K);
			}
			i++;
		}

		free(indices);
		free(heap_data);
		free(last_added_data);
		free(last_added_indices);
	}

end:

	free(aux_data);
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
				if (next_count > N) /* this means we're done */
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
				if (next_count > N) /* this means we're done */
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
insertion(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = 1; i < N; i++)
	{
		int j = i;
		v = a[i];

		while((j > 0) && less(v, a[j-1]))
		{
			a[j] = a[j-1];
			j--;
		}
		a[j] = v;
	}
}
static void
insertion_reverse(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = N-2; i >= 0; i--)
	{
		int j = i;
		v = a[i];

		while((j < N-1) && less(v, a[j+1]))
		{
			a[j] = a[j+1];
			j++;
		}
		a[j] = v;
	}
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

