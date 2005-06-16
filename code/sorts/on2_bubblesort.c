#include "on2_sorts.h"

void
on2_bubblesort(unsigned int a[], int N)
{
	int i, j;
	int sorted;

	char string[10];

	for (i = 0; i < N-1; i++)
	{
		sprintf(string, "%d", i);
		describe_predictor(&global_predictor[i], string);

		sorted = 1;
		for (j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				branch_taken(&global_predictor[i]);
				exch(a[j-1],a[j]);
				sorted = 0;
			}
			branch_not_taken(&global_predictor[i]);
			
		}
		if (sorted) return;
	}
}
