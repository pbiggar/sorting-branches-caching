#include <stdlib.h>

#include "base_sorts.h"

#define bitsword 32
#define bitsbyte 8
#define bytesword 4
#define Radix (1 << bitsbyte)

#define digit(A, B) (((A) >> (bitsword-((B)+1)*bitsbyte)) & (Radix-1))

void
base_radixsort(unsigned int a[], int N)
{
	unsigned int* radix_aux = malloc(N * sizeof(unsigned int));
	int r = N-1;
	int l = 0;
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

	free(radix_aux);
}
