#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base_sorts.h"
#include "insertion.c"

void
base_mergesort(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int)); /* make it twice the size to use the notation */

	int s = 0; /* this picks which area we write to */
	
	int i,j; /* indices for the first array */
	int k, l; /* indices for second array */
	
	int d, f; /* d => direction, if (f == 0) keep going */

	unsigned int temp;

	unsigned int* source;
	unsigned int* target;
	
N2:	/* Prepare for pass */

	i = 0;
	j = N-1;
	k = 0;
	l = N-1;
	d = 1;
	f = 1;

	if (s == 0)
	{
		source = a;
		target = aux;
	}
	else
	{
		source = aux;
		target = a;
	}
	

/*	printf("2,3\n"); */

N3: /* compare Ki, Kj */

	if (source[i] > source[j])
	{
/*		printf("3,8\n"); */
		goto N8;
	}

	if (i == j)
	{
		target[k] = source[i];
/*		printf("3,13\n"); */
		goto N13;
	}

/*N4:  transmit Ri */

/*	printf("3,4\n"); */

	target[k] = source[i];
	k = k + d; /* increment in the correct direction */

/*N5: // Stepdown? */

	i++;
	if (source[i-1] <= source[i])
	{
/*		printf("4,3\n"); */
		goto N3;
	}

/*	printf("4,6\n"); */
N6: 
	target[k] = source[j];
	k = k + d;

/*N7: // stepdown? */

	j--;
	if (source[j+1] <= source[j])
	{
/*		printf("6,6\n"); */
		goto N6;
	}
	else
	{
/*		printf("6,12\n"); */
		goto N12;
	}

N8: /* transmit Rj */

	target[k]  = source[j];
	k = k + d; /* increment in the correct direction */

/*N9: // Stepdown? */

	j--;
	if (source[j+1] <= source[j])
	{
/*		printf("8,3\n"); */
		goto N3;
	}

/*	printf("8,10\n"); */
N10: /* transmit Ri */


	target[k] = source[i];
	k = k + d;

/*N11: // stepdown? */

	i++;
	if (source[i-1] <= source[i])
	{
/*		printf("10,10\n"); */
		goto N10;
	}

/*	printf("10,12\n"); */
N12: /* switch sides (of the flow graph on page 162) */

	f = 0;
	d = -d; /* change the direction */
	temp = k;
	k = l;
	l = temp;

/*	printf("12,3\n"); */

	goto N3;
	

N13: /* switch areas */

	if (f == 0)
	{
		s = 1 - s; /* s = !s */
/*		printf("13,2\n"); */
		goto N2;
	}
	else /* sorting is complete */
	{
/*		printf("s = %d\n", s);
		exit(0); */ 
		if (s == 0)
		{
			memcpy(a, target, N * sizeof(unsigned int)); 
		}
	}

	free(aux);
}
