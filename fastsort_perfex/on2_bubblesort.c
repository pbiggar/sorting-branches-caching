#include "on2_sorts.h"
#include <stdio.h>

void
on2_bubblesort(unsigned int a[], int N)
{
	int i, j;
	int sorted;

	for (i = 0; i < N-1; i++)
	{
		sorted = 1;
		for (j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				exch(a[j-1],a[j]);
				sorted = 0;
			}
			
		}
		if (sorted) return;
	}
}
