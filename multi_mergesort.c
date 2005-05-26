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
		branch_taken(&global_predictor[33]);
		heap[child] = heap[parent];
		heap[parent] = v;
		child = parent;
	}
	branch_not_taken(&global_predictor[33]);
}

static void
fix_down(unsigned int heap[], int parent, int N)
{
	int child;
	int child2;
	unsigned int v = heap[parent];
/*	printf("fixdown when N = %d\n", N); */
	while(1) /* see if it has any children */
	{
		if (parent*HEAP_SIZE+1 < N) branch_taken(&global_predictor[34]);
		else
		{
			branch_not_taken(&global_predictor[34]);
			break;
		}
		child = parent*HEAP_SIZE + 1; /* this gets the rightmost node */
		child2 = child;

		if (less(heap[child2 + 0], heap[child]))
		{
			child = child2 + 0;
			branch_taken(&global_predictor[35]);
		}
		else branch_not_taken(&global_predictor[35]);
		if (less(heap[child2 + 1], heap[child]))
		{
			child = child2 + 1;
			branch_taken(&global_predictor[36]);
		}
		else branch_not_taken(&global_predictor[36]);
		if (less(heap[child2 + 2], heap[child]))
		{
			child = child2 + 2;
			branch_taken(&global_predictor[37]);
		}
		else branch_not_taken(&global_predictor[37]);
		if (less(heap[child2 + 3], heap[child]))
		{
			child = child2 + 3;
			branch_taken(&global_predictor[38]);
		}
		else branch_not_taken(&global_predictor[38]);

#if (HEAP_SIZE == 8)
		if (less(heap[child2 + 4], heap[child]))
		{
			child = child2 + 4;
			branch_taken(&global_predictor[39]);
		}
		else branch_not_taken(&global_predictor[39]);
		if (less(heap[child2 + 5], heap[child]))
		{
			child = child2 + 5;
			branch_taken(&global_predictor[40]);
		}
		else branch_not_taken(&global_predictor[40]);
		if (less(heap[child2 + 6], heap[child]))
		{
			child = child2 + 6;
			branch_taken(&global_predictor[41]);
		}
		else branch_not_taken(&global_predictor[41]);
		if (less(heap[child2 + 7], heap[child]))
		{
			child = child2 + 7;
			branch_taken(&global_predictor[42]);
		}
		else branch_not_taken(&global_predictor[42]);

#endif

		/* when the larger child isnt large enough to be promoted, stop */
		if (!less(heap[child], v))
		{
			branch_taken(&global_predictor[43]);
			break;
		}
		branch_not_taken(&global_predictor[43]);

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

	describe_predictor(&global_predictor[0], "presort 0");
	describe_predictor(&global_predictor[1], "presort 1");
	describe_predictor(&global_predictor[2], "presort 2");
	describe_predictor(&global_predictor[3], "presort 3");
	describe_predictor(&global_predictor[4], "presort 4");
	describe_predictor(&global_predictor[5], "presort 5");
	describe_predictor(&global_predictor[6], "presort 6");
	describe_predictor(&global_predictor[7], "presort 7");
	describe_predictor(&global_predictor[8], "presort 8");
	describe_predictor(&global_predictor[9], "presort 9");
	describe_predictor(&global_predictor[10], "presort 10");
	describe_predictor(&global_predictor[11], "presort 11");
	describe_predictor(&global_predictor[12], "forwards left 0");
	describe_predictor(&global_predictor[13], "forwards left 1");
	describe_predictor(&global_predictor[14], "forwards left 2");
	describe_predictor(&global_predictor[15], "forwards left 3");
	describe_predictor(&global_predictor[16], "forwards left 4");
	describe_predictor(&global_predictor[17], "forwards left 5");
	describe_predictor(&global_predictor[18], "forwards left 6");
	describe_predictor(&global_predictor[19], "forwards left 7");
	describe_predictor(&global_predictor[20], "forwards right 0");
	describe_predictor(&global_predictor[21], "forwards right 1");
	describe_predictor(&global_predictor[22], "forwards right 2");
	describe_predictor(&global_predictor[23], "forwards right 3");
	describe_predictor(&global_predictor[24], "forwards right 4");
	describe_predictor(&global_predictor[25], "forwards right 5");
	describe_predictor(&global_predictor[26], "forwards right 6");
	describe_predictor(&global_predictor[27], "forwards right 7");
	describe_predictor(&global_predictor[28], "forwards middle");
	describe_predictor(&global_predictor[29], "forwards next");
	describe_predictor(&global_predictor[30], "forwards end");
	describe_predictor(&global_predictor[31], "forwards setup");
	describe_predictor(&global_predictor[32], "forwards equal");
	describe_predictor(&global_predictor[33], "has children");
	describe_predictor(&global_predictor[34], "comparison 0");
	describe_predictor(&global_predictor[35], "comparison 1");
	describe_predictor(&global_predictor[36], "comparison 2");
	describe_predictor(&global_predictor[37], "comparison 3");
	describe_predictor(&global_predictor[38], "comparison 4");
	describe_predictor(&global_predictor[39], "comparison 5");
	describe_predictor(&global_predictor[40], "comparison 6");
	describe_predictor(&global_predictor[41], "comparison 7");
	describe_predictor(&global_predictor[42], "child promotion");
	describe_predictor(&global_predictor[43], "fix up");
	describe_predictor(&global_predictor[44], "tag");
	describe_predictor(&global_predictor[45], "reverse? tag");


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
			if (less(a[i+2], a[j]))
			{
				branch_taken(&global_predictor[0]);
				j = i+2;
			}
			else branch_not_taken(&global_predictor[0]);
			if (less(a[i+3], a[j]))
			{
				branch_taken(&global_predictor[1]);
				j = i+3;
			}
			else branch_not_taken(&global_predictor[1]);
			if (less( a[j], a[i]))
			{
				branch_taken(&global_predictor[2]);
				exch(a[i],  a[j]);
			}
			else branch_not_taken(&global_predictor[2]);

			j = i+2;
			if (less(a[i+3], a[j]))
			{
				branch_taken(&global_predictor[3]);
				j = i+3;
			}
			else branch_not_taken(&global_predictor[3]);
			if (less(a[j], a[i+1]))
			{
				branch_taken(&global_predictor[4]);
				exch(a[i+1], a[j]);
			}
			else branch_not_taken(&global_predictor[4]);

			if(less(a[i+3], a[i+2]))
			{
				branch_taken(&global_predictor[5]);
				exch(a[i+2], a[i+3]);
			}
			else branch_not_taken(&global_predictor[5]);

			j = i+5;

			if (less(a[j], a[i+6]))
			{
				branch_taken(&global_predictor[6]);
				j = i+6;
			}
			else branch_not_taken(&global_predictor[6]);
			if (less(a[j], a[i+7]))
			{
				branch_taken(&global_predictor[7]);
				j = i+7;
			}
			else branch_not_taken(&global_predictor[7]);
			if(less(a[i+4], a[j]))
			{
				branch_taken(&global_predictor[8]);
				exch(a[i+4], a[j]);
			}
			else branch_not_taken(&global_predictor[8]);

			j = i+6;
			if (less(a[j], a[i+7]))
			{
				branch_taken(&global_predictor[9]);
				j = i+7;
			}
			else branch_not_taken(&global_predictor[9]);
			if(less(a[i+5], a[j]))
			{
				branch_taken(&global_predictor[10]);
				exch(a[i+5], a[j]);
			}
			else branch_not_taken(&global_predictor[10]);

			if(less(a[i+6], a[i+7]))
			{
				branch_taken(&global_predictor[11]);
				exch(a[i+6], a[i+7]);
			}
			else branch_not_taken(&global_predictor[11]);
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
				{
					branch_taken(&global_predictor[12]);
					break;
				}
				branch_not_taken(&global_predictor[12]);
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
				{
					branch_taken(&global_predictor[13]);
					break;
				}
				branch_not_taken(&global_predictor[13]);
				target[k] = source[i];
				k += d;
				i++;

				if (source[i] >= source[j])
				{
					branch_taken(&global_predictor[14]);
					break;
				}
				branch_not_taken(&global_predictor[14]);
				target[k] = source[i];
				k += d;
				i++;

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
			}

			/* iterate through the right list */
			while(1)
			{
				if (source[i] <= source[j])
				{
					branch_taken(&global_predictor[20]);
					break;
				}
				branch_not_taken(&global_predictor[20]);
				target[k] = source[j];
				k += d;
				j--;

				if (source[i] <= source[j])
				{
					branch_taken(&global_predictor[21]);
					break;
				}
				branch_not_taken(&global_predictor[21]);
				target[k] = source[j];
				k += d;
				j--;

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
			}

			/* check if we're in the middle */
			if(i == j)
			{
				branch_taken(&global_predictor[28]);
				target[k] = source[i];
				i = track + next_count;

				/* check whether we have left the building */
				if (i >= limit)
				{
					branch_taken(&global_predictor[29]);
					if (next_count >= (limit - base)) /* this means we're done */
					{
						branch_taken(&global_predictor[30]);
						break;
					}
					branch_not_taken(&global_predictor[30]);

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
				branch_not_taken(&global_predictor[29]);

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
					branch_taken(&global_predictor[31]);
					d = -1;
					k = j;
				}
				else
				{
					branch_not_taken(&global_predictor[31]);
					d = 1;
					k = i;
				}
			}
			else 
			{
				branch_not_taken(&global_predictor[28]);
				if (source[i] == source[j])
				{
					branch_taken(&global_predictor[32]);
					target[k] = source[i];
					i++;
					k += d;
				}
				else branch_not_taken(&global_predictor[32]);
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
				branch_taken(&global_predictor[44]);
				if (last_added_index & 1)
				{
					int space_to_end = indices[last_added_index] - indices[last_added_index - 1] + 1; /* we add 1 here because both indices are on an item that hasnt yet been added */
					branch_taken(&global_predictor[45]);
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
					branch_not_taken(&global_predictor[45]);
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
			else
			{
				branch_not_taken(&global_predictor[44]);
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
