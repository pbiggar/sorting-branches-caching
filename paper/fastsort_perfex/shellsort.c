#define Item unsigned int
#include "utils.h"

void shellsort(Item a[], int N)
{
	int i, h,min, bound; 
	for (h = 5*N/11; h >= 5; h = 5*h/11)
	{
		for (i = h; i <= N-1; i++)
		{
			int j = i; 
			Item v = a[i]; 
			while (j >= h && less(v, a[j-h]))
			{ 
				a[j] = a[j-h]; 
				j -= h; 
			}
			a[j] = v; 
		}
	}

	/* put a sentinel in place */
	/* the sentinel will be in the first h positions */
	/* for h to be below 5, it must have been less than 11 beforehand */
	min = 0;
	bound = 11;
	if (N < bound) bound = N;

	for(i = 1; i < bound; i++)
	{
		if(less(a[i], a[min]))
		{
			min = i;
		}
	}
	exch(a[0], a[min]);


	for(i = 1; i < N; i++)
	{
		int j = i;
		Item v = a[i];

		while(less(v, a[j-1]))
		{
			a[j] = a[j-1];
			j--;
		}
		a[j] = v;
	}
}
