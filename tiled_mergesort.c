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
