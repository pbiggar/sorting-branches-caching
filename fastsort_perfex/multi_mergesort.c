#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <limits.h>

#include "cache_sorts.h"

#define LIMIT OUT_OF_PLACE_LIMIT
#define LIMIT_BITS OUT_OF_PLACE_LIMIT_BITS

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

static void
sort_last_index(unsigned int a[], unsigned int aux[], int N)
{
	int i;
	unsigned int v, v2;
	for(i = 1; i < N; i++)
	{
		int j = i;
		v = a[i];
		v2 = aux[i];

		while(less(v, a[j-1]))
		{
			a[j] = a[j-1];
			aux[j] = aux[j-1];
			j--;
		}
		a[j] = v;
		aux[j] = v2; /* a[i] doesnt change at all */
	}
}


void
multi_mergesort(unsigned int a[], int N)
{
	/* track keeps an eye on where i started last */
	/* outer track keeps and eye on which cache_sized/2 segemnt i started in last */
	int i,j,track; /* indices for the first array */
	int k; /* indices for second array */
	int min;
	
	int d, outer_d; /* d => direction */

	int count, next_count; /* how many in this list */

	int base, limit, last;

	unsigned int* temp_pointer;
	unsigned int temp;

	unsigned int *aux, *aux_data, *source, *target, *heap_data, *heap, *last_added;
	int *indices, *last_added_indices, *last_added_data;

	/* get the address we need */
	unsigned int minusA = ((1 << BLOCK_BITS) - get_index(a));
	aux_data = memalign(ALIGNMENT, (N + 2*LIMIT) * sizeof(unsigned int));
	aux = (unsigned int*)(((unsigned int)aux_data & (~BLOCK_AND_LINE_MASK)) | (minusA << LINE_BITS));
	if (aux < aux_data) /* then the new index is less than the old one */
	{
		aux = (unsigned int*)((unsigned int)aux + (1 << (BLOCK_AND_LINE_BITS)));
	}

	base = 0;
	last = 0;
	outer_d = 0;

	if (N <= LIMIT) /* if it can be done inside the cache */
	{
		limit = N;
		base = 0;
		last = 1;
		outer_d = 1;

		goto small;
	}

	do
	{
		limit = base + LIMIT;
		outer_d = 1 - outer_d;

small: /* we skip here if the data set fits in half the cache */
		source = a;
		target = aux;

		if (limit > N) limit = N;

		/* pre-sort */ 
		for(i = base; i <= limit-8; i+=8)
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

		if (limit < temp) temp = limit;
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
		for(i = temp; i < limit-1; i++)
		{
			min = i;
			for(j = i+1; j < limit; j++)
			{
				if(!less(a[j],a[min]))
				{
					min = j;
				}
			}
			exch(a[i], a[min]);
		}

		count = 4;
		next_count = 8;

		i = base;
		track = i;
		k = i;
		d = 1;
		j = i + next_count - 1;
		if (j >= limit - 1)
		{
			j = limit-1;
			if (!last && (outer_d == 0)) /* if theres only 1 copy, reverse here */
			{
				/* its if this is the rightmost set */
				k = j;
				d = -1;
			}
		}

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
				if (i >= limit)
				{
					/* HACK i changed this from a >= to > so that small merges will be bitonic */
					if (next_count > (limit - base)) /* this means we're done */
					{
						break;
					}

					/* the next iteration will be over bigger lists */
					count = next_count;
					next_count <<= 1;

					temp_pointer = source;
					source = target;
					target = temp_pointer;

					i = base;
					j = i + next_count - 1;
					k = i;
					d = 1;
					if (j >= limit-1) /* if this sort is smaller than we designed for */
					{
						j = limit-1;
						if (!last && (outer_d == 0)) /* go backwards */
						{
							d = -1;
							k = j;
						}
					}
					track = i;

					if (next_count == LIMIT) /* we want to hit the last copy only (not the rightmost) */
					{
						if (outer_d == 0) /* go backwards */
						{
							d = -1;
							k = j;
						}
					}

					/* the last iteration will be backwards to keep it bitonic */
					continue;
				}

				j = i + next_count - 1;
				if (j >= limit-1)
				{
					/* since this is in the middle of a merge, we dont change direction in the event of j going over */
					j = limit-1;
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

		base += LIMIT;
	}
	while((base < N) && (last == 0));

#define K (((N-1) >> (LIMIT_BITS))+1)

#ifdef DEBUG
	print_int_array(source, N, "source", -1, -1, -1);
	print_int_array(target, N, "target", -1, -1, -1);
#endif
	/* now we do the k-way mergesort */
	if (N > LIMIT) /* else its already sorted */
	{
	 /* TODO align this as a heap should be */
		int heap_length = 0;
		indices = memalign(HEAP_SIZE, ((K+1)^1) * sizeof(int)); /* this changes to an even number (rounding up) */
		heap_data = memalign(HEAP_SIZE, (K+1) * HEAP_SIZE * sizeof(unsigned int));
		heap = heap_data + (HEAP_SIZE - 1);
		memset(heap_data, 0xff, (K+1) * HEAP_SIZE * sizeof(unsigned int));
		last_added_data = memalign(HEAP_SIZE, (K+1) * sizeof(int));
		last_added_indices = memalign(HEAP_SIZE, K * sizeof(unsigned int));

		last_added = &last_added_data[1];
		last_added_data[0] = 0;

		temp_pointer = source;
		source = target;
		target = temp_pointer;


		/* we add 8 from each to the Q */
		for(i = 0; i < K; i++) /* we do the last one ourselves */
		{
			if (i & 1) /* this is a reverse list */
			{
				if (i == K-1) indices[i] = N-1; /* last mini-list */
				else indices[i] = ((i+1) << LIMIT_BITS) - 1;

				add_heap_line(&source[indices[i]], heap, -1, heap_length, HEAP_SIZE);
				indices[i] -= HEAP_SIZE;

				last_added[i] = source[indices[i]+1];
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
					add_heap_line(&source[indices[i]], heap, 1, heap_length, space_to_end); /* FIXME if there arent 8 in the last list this could be awkward */
					indices[i] += space_to_end;
				}
				else
				{
					add_heap_line(&source[indices[i]], heap, 1, heap_length, HEAP_SIZE);
					indices[i] += HEAP_SIZE;
				}

				last_added[i] = source[indices[i]-1];
				last_added_indices[i] = i;
			}
			heap_length += HEAP_SIZE;
		}

		sort_last_index(last_added, last_added_indices, K);

		/* do the merge */
		i = 0;
		while(i < N)
		{
			target[i] = heap[0];
			heap[0] = heap[heap_length-1];
			heap[heap_length-1] = UINT_MAX;
			fix_down(heap, 0, --heap_length);

			if (target[i] == last_added[0])
			{
				int last_added_index = last_added_indices[0];
				if (last_added_index & 1)
				{
					int space_to_end = indices[last_added_index] - indices[last_added_index - 1] + 1; /* we add 1 here because both indices are on an item that hasnt yet been added */
					if (space_to_end < HEAP_SIZE) /* bounds check (l_a_i must have a buddy) */
					{
						add_heap_line(&source[indices[last_added_index]], heap, -1, heap_length, space_to_end);
						indices[last_added_index] -= space_to_end;
						last_added[0] = UINT_MAX;
						heap_length += space_to_end;
					}
					else
					{
						add_heap_line(&source[indices[last_added_index]], heap, -1, heap_length, HEAP_SIZE);
						indices[last_added_index] -= HEAP_SIZE;
						last_added[0] = source[indices[last_added_index]+1];
						heap_length += HEAP_SIZE;
					}
				}
				else
				{
					int space_to_end = indices[last_added_index+1] - indices[last_added_index] + 1;
					/* the will have a buddy on the right */
					if (space_to_end < HEAP_SIZE)
					{
						add_heap_line(&source[indices[last_added_index]], heap, 1, heap_length, space_to_end);
						indices[last_added_index] += space_to_end;
						last_added[0] = UINT_MAX;
						heap_length += space_to_end;
					}
					else
					{
						add_heap_line(&source[indices[last_added_index]], heap, 1, heap_length, HEAP_SIZE);
						indices[last_added_index] += HEAP_SIZE;
						last_added[0] = source[indices[last_added_index]-1];
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

	/* copy back if its in the wrong array */
	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux_data);
}
