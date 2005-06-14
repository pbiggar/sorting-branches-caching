#include "on2_sorts.h"

void
on2_bubblesort2(unsigned int a[], int N)
{
	int i = 0, j, k = 0;

	while(i < N-1)
	{
		k = N-1;
		for (j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				k = j;
				exch(a[j-1],a[j]);
			}
		}
		i = k;
	}
}
