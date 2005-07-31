#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "cache_sorts.h"

#define LIMIT OUT_OF_PLACE_LIMIT
#define LIMIT_BITS OUT_OF_PLACE_LIMIT_BITS

#define THRESHHOLD 10

#define BITSWORD 32
#define BITSBYTE 8
#define BYTESWORD 4
#define R 256

#define digit(A, B) (((A) >> (BITSWORD-((B)+1)*BITSBYTE)) & (R-1))

void
aligned_cache_radixsort(unsigned int a[], int N)
{
	unsigned int* aux_data;
	unsigned int* aux;
	int i, j, w; /* R is the size of a digit, hence the number of bins */
	int count0[R+1];
	int count1[R+1];
	int* temp;

	int* count = count0;
	int* next_count = count1;

	unsigned int* s;
	unsigned int* t;

	unsigned int minusA;

	/* get the index we need*/
	minusA = get_index(a) ^ (1 << (BLOCK_BITS - 1));

	/* also align it for level 1 misses */
	minusA  = minusA ^ (1 << (LEVEL1_BLOCK_BITS - 1));

	aux_data = memalign(ALIGNMENT, (N + 2*LIMIT) * sizeof(unsigned int));
	aux = (unsigned int*)(((unsigned int)aux_data & (~BLOCK_AND_LINE_MASK)) | (minusA << LINE_BITS));
	if (aux < aux_data) /* then the new index is less than the old one */
	{
		aux = (unsigned int*)((unsigned int)aux + (1 << (BLOCK_AND_LINE_BITS)));
	}

	s = a;
	t = aux;

	/* count the digits for the first time */
	for(j = 0; j < R; j++) count[j] = 0;
	for(i = 0; i < N; i++) count[digit(s[i], 3) + 1]++;
	for(j = 1; j < R; j++) count[j] += count[j-1];

	for(w = BYTESWORD - 1; w > 0; w--) /* this is from lsb. 3 == lsByte, 0 == msByte */
	{
		for(j = 0; j < R; j++) next_count[j] = 0; /* set the next counts to 0 */

		for(i = 0; i <= N-1; i++) /* when we look up the array, we now have its position */
		{
			t[count[digit(s[i], w)]++] = s[i];
			next_count[digit(s[i], w-1) + 1]++;
		}
	
		for(j = 1; j < R; j++) next_count[j] += next_count[j-1]; /* make the next counts cumulative */

		/* swap the counts */
		temp = next_count;
		next_count = count;
		count = temp;;

		/* swap the arrays */
		temp = t;
		t = s;
		s = temp;
	}

	/* finish it off */
	for(i = 0; i <= N-1; i++) /* when we look up the array, we now have its position */
	{
		t[count[digit(s[i], w)]++] = s[i];
	}

	free(aux_data);
}
