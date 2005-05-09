#include <stdlib.h>

#include "base_sorts.h"
#include "insertion.c"

#define THRESHHOLD 10

#define bitsword 32
#define bitsbyte 8
#define bytesword 4
#define Radix (1 << bitsbyte)

#define digit(A, B) (((A) >> (bitsword-((B)+1)*bitsbyte)) & (Radix-1))

/* this gets the MSB */
/*#define digit(A, B) (((A) >> (bitsword-((B)+1))) & 1) */

static void
quicksortB(unsigned int a[], int l, int r, int w)
{
	int i = l;
	int j = r;

	if (r <= l || w > bitsword) return;

	while(j != i)
	{
		while(digit(a[i], w) == 0 && (i<j))
			i++;
		while(digit(a[j], w) == 1 && (j>i))
			j--;
		exch(a[i], a[j]);
	}
	if (digit(a[r], w) == 0) j++;
	quicksortB(a, l, j-1, w+1);
	quicksortB(a, j, r, w+1);
}


static unsigned int* radix_aux;

#define bin(A) (l+count[(A)])
static void
radixMSD(unsigned int a[], int l, int r, int w)
{
/*	printf("\nl:%d, r=%d, W:%d\n", l, r, w); */
	int i, j;
	int count[Radix+1];

	if (w > bytesword) return;
	if (r-l <= THRESHHOLD)
	{
		insertion_sentinel(&a[l], r-l);
		insertion(&a[l], r-l);
		return;
	}

	for(j = 0 ; j < Radix; j++) count[j] = 0; /* clear count */
	
	for(i = l; i <= r; i++)
		count[digit(a[i], w) + 1]++; /* count the items with these digits */

	for(j = 1; j < Radix; j++)
		count[j] += count[j-1]; /* cumulative */

	for(i = l; i <= r; i++)
		radix_aux[l+count[digit(a[i], w)]++] = a[i]; /* copy into correct places */

	for(i = l; i <= r; i++) 
		a[i] = radix_aux[i]; /* copy back */

	radixMSD(a,l, bin(0)-1, w+1);

	for(j = 0; j < Radix-1; j++)
		radixMSD(a, bin(j), bin(j+1)-1, w+1);
}

static void
radixLSD(unsigned int a[], int l, int r)
{
	int i, j, w, count[Radix+1]; /* R is the size of a digit, hence the number of bins */

	for(w = bytesword - 1; w >= 0; w--) /* this is from lsb. 3 == lsByte, 0 == msByte */
	{
		for(j = 0; j < Radix; j++) count[j] = 0; /* set the counts to 0 */
		for(i = l; i <= r; i++) /* count each digit */
		{
			count[digit(a[i], w) + 1]++;
		}
		for(j = 1; j < Radix; j++) /* make the count cumulatative */
		{
			count[j] += count[j-1];
		}
		for(i = l; i <= r; i++) /* when we look up the array, we now have its position */
		{
			radix_aux[count[digit(a[i], w)]++] = a[i];
		}
		for(i = l; i <= r; i++) /* copy it back */
		{
			a[i] = radix_aux[i];
		}
	}
}

void
base_radixsort(unsigned int a[], int N)
{
	radix_aux = malloc(N * sizeof(unsigned int));
	radixLSD(a, 0, N-1);
	free(radix_aux);
}
