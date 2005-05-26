#include "on2_sorts.h"

void
on2_shakersort2(unsigned int a[], int N)
{
	int i = 0, j;
	int k = N-1;
	int l = N-1;

	for ( ; i < k; )
	{
		for (j = k; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				exch(a[j-1],a[j]);
				l = j;
			}
		}
		i = l;

		for (j = i ; j < k; j++)
		{
			if (less(a[j+1],a[j]))
			{
				exch(a[j],a[j+1]);
				l = j;
			}
		}
		k = l;
		
/*		if (sorted) return; */
	}
}
