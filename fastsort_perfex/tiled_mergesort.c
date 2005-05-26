#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "cache_sorts.h"

#define LIMIT OUT_OF_PLACE_LIMIT

void
tiled_mergesort(unsigned int a[], int N)
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

	unsigned int* aux_data;
	unsigned int* aux;
	unsigned int *source, *target;

	/* get the address we need*/
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

final:
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
					if (next_count >= (limit - base)) /* this means we're done */
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

	/* now we set the values properly and merge the final sets */
	if (last == 0)
	{
		/* the rightmost one may be smaller than the rest, and so may not be in the correct target.  */
		/* In this case we should copy over */

		temp_pointer = target;
#if (LIMIT_BITS & 1)
		source = aux;
		target = a;
#else
		source = a;
		target = aux;
#endif

		if (temp_pointer != source)
		{
			i = base - LIMIT;
			j = N - i;
			memcpy(&source[i], &temp_pointer[i], j * sizeof(unsigned int));
		}

		count = LIMIT;
		next_count = LIMIT << 1;
		limit = N;
		base = 0;
		last = 1;
		outer_d = 1; /* forwards */

		goto final;
	}

	/* copy back if its in the wrong array */
	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux_data);
}
