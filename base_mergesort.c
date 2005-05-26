#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base_sorts.h"

void
base_mergesort(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int)); /* make it twice the size to use the notation */

	int i,j; /* indices for the first array */
	int k; /* indices for second array */
	
	int d; /* d => direction, if (f == 0) keep going */

	int next_count;

	int track; /* this keeps track of i so we know where to merge next */
	/* an alternative would be to add count/2 and clear the lsb. */

	unsigned int* temp_pointer;
	unsigned int temp;
	int min;

	unsigned int* source = a;
	unsigned int* target = aux;

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
	describe_predictor(&global_predictor[12], "middle");
	describe_predictor(&global_predictor[13], "next");
	describe_predictor(&global_predictor[14], "end");
	describe_predictor(&global_predictor[15], "setup");
	describe_predictor(&global_predictor[16], "equal");
#ifdef _USE_ROLLED_LOOPS
	describe_predictor(&global_predictor[17], "left");
	describe_predictor(&global_predictor[18], "right");
#else
	describe_predictor(&global_predictor[17], "left 0");
	describe_predictor(&global_predictor[18], "left 1");
	describe_predictor(&global_predictor[19], "left 2");
	describe_predictor(&global_predictor[20], "left 3");
	describe_predictor(&global_predictor[21], "left 4");
	describe_predictor(&global_predictor[22], "left 5");
	describe_predictor(&global_predictor[23], "left 6");
	describe_predictor(&global_predictor[24], "left 7");
	describe_predictor(&global_predictor[25], "right 0");
	describe_predictor(&global_predictor[26], "right 1");
	describe_predictor(&global_predictor[27], "right 2");
	describe_predictor(&global_predictor[28], "right 3");
	describe_predictor(&global_predictor[29], "right 4");
	describe_predictor(&global_predictor[30], "right 5");
	describe_predictor(&global_predictor[31], "right 6");
	describe_predictor(&global_predictor[32], "right 7");
#endif

/*	int remainder = (N & 7); */

/*	print_int_array(a, N, "before pre-sorting", 0, -1, -1); */
	/* pre-sort */
	for(i = 0; i <= N-8; i+=8)
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


	/* now beging merging */
	track = 0;
	next_count = 8;

	i = 0;
	j = next_count - 1;
	k = 0;
	d = 1;

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[17]);
				break;
			}
			branch_not_taken(&global_predictor[17]);
			target[k] = source[i];
			k += d;
			i++;
#ifndef _USE_ROLLED_LOOPS

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

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[22]);
				break;
			}
			branch_not_taken(&global_predictor[22]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[23]);
				break;
			}
			branch_not_taken(&global_predictor[23]);
			target[k] = source[i];
			k += d;
			i++;

			if (source[i] >= source[j])
			{
				branch_taken(&global_predictor[24]);
				break;
			}
			branch_not_taken(&global_predictor[24]);
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
				branch_taken(&global_predictor[18]);
				break;
			}
			branch_not_taken(&global_predictor[18]);
			target[k] = source[j];
			k += d;
			j--;
#else
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

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[30]);
				break;
			}
			branch_not_taken(&global_predictor[30]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[31]);
				break;
			}
			branch_not_taken(&global_predictor[31]);
			target[k] = source[j];
			k += d;
			j--;

			if (source[i] <= source[j])
			{
				branch_taken(&global_predictor[32]);
				break;
			}
			branch_not_taken(&global_predictor[32]);
			target[k] = source[j];
			k += d;
			j--;
#endif
		}

		/* check if we're in the middle */
		if(i == j)
		{
			branch_taken(&global_predictor[12]);
			target[k] = source[i];
			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				branch_taken(&global_predictor[13]);
				if (next_count >= N) /* this means we're done */
				{
					branch_taken(&global_predictor[14]);
					break;
				}
				branch_not_taken(&global_predictor[14]);

				/* the next iteration will be oevr bigger lists */
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
			else branch_not_taken(&global_predictor[13]);

			j = i + next_count - 1;
			if (j >= N)
			{
				j = N-1;
			}

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				branch_taken(&global_predictor[15]);
				d = -1;
				k = j;
			}
			else
			{
				branch_not_taken(&global_predictor[15]);
				d = 1;
				k = i;
			}
		}
		else 
		{
			branch_not_taken(&global_predictor[12]);
			if (source[i] == source[j])
			{
				branch_taken(&global_predictor[16]);
				target[k] = source[i];
				i++;
				k += d;
			}
			else branch_not_taken(&global_predictor[16]);
		}
	}


	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);

	// which results arent interesting
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
	init_predictor(&global_predictor[14]);
	init_predictor(&global_predictor[15]);
	init_predictor(&global_predictor[16]);
}
