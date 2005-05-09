#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void knuth_other_base_mergesort(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int));
	int s; /* s keeps track of the target, we can write something to remove this */
	int i,j,k,l;
	int d; /* direction counter */
	
	int temp;

	int p, q, r; /* p keeps count of the number of items in this merge, q and r are counters for ascending and descending */
	unsigned int* source;
	unsigned int* target;

/*S1:*/
	s = 0; /* we should be able to elimnate s from here */
	p = 1;

S2:
	i = 0;
	j = N-1;
	k = -1;
	l = N;
	d = 1;
	q = p;
	r = p;

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

S3:
	if (source[i] > source[j])
		goto S8;

/*S4:*/
	k = k + d;
	target[k] = source[i];

/*S5:*/
	i = i+1;
	q = q-1;
	if (q > 0)
		goto S3;

S6:
	k = k+d;
	if (k == l)
		goto S13;

	target[k] = source[j];

/*S7: */
	j = j-1;
	r = r-1;
	if (r>0)
		goto S6;
	
	goto S12;

S8:
	k = k + d;
	target[k] = source[j];

/*S9:*/
	j = j-1;
	r = r-1;
	if (r > 0)
		goto S3;

S10:
	k = k+d;
	if (k == l)
		goto S13;

	target[k] = source[i];

/*S11: */
	i = i+1;
	q = q-1;
	if(q > 0)
		goto S10;

S12:
	q = p;
	r = p;
	d = -d;
	temp = k; /* swap */
	k = l;
	l = temp;
	if (j-i<p)
		goto S10;

	goto S3;

S13:
	p = p+p;
	if (p < N)
	{
		s = 1-s;
		goto S2;
	}

	if (s==0)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);
}
