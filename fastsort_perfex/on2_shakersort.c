#include "on2_sorts.h"

void
on2_shakersort(unsigned int a[], int N)
{
	int i, j;
	int k = N-1;
	int sorted;

	for (i = 0; i < k;  )
	{
		sorted = 1;
		for (j = k; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				exch(a[j-1],a[j]);
				sorted = 0;
			}
		}
		i++;
		for (j = i ; j < k; j++)
		{
			if (less(a[j+1],a[j]))
			{
				exch(a[j],a[j+1]);
				sorted = 0;
			}
		}
		if (sorted) return;
		k--;
	}
}
