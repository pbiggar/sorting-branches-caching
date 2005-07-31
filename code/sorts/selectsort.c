#include "on2_sorts.h"

void
selectsort(unsigned int a[], int N)
{ 
	int i, j;
	for (i = 0; i < N-1; i++)
	{ 
		int min = i;
		for (j = i+1; j < N; j++) 
		{
			if (less(a[j], a[min])) min = j;
		}
		exch(a[i], a[min]);
	} 
}
