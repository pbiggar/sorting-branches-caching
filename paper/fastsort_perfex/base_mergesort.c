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

/*	int remainder = (N & 7); */

/*	print_int_array(a, N, "before pre-sorting", 0, -1, -1); */
	/* pre-sort */
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


	if (target != a)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);
}
