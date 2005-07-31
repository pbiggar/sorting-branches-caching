#include "on2_sorts.h"

void
on2_bubblesort(unsigned int a[], int N)
{
	int i, j;
	int sorted;

#ifdef _USE_SOFTWARE_PREDICTOR
	char string[10];
#endif

	for (i = 0; i < N-1; i++)
	{
#ifdef _USE_SOFTWARE_PREDICTOR
		sprintf(string, "%d", i);
		describe_predictor(&global_predictor[i], string);
#endif

		sorted = 1;
		for (j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				branch_taken(&global_predictor[i]);
				exch(a[j-1],a[j]);
				sorted = 0;
			}
			else
			{
				branch_not_taken(&global_predictor[i]);
			}
			
		}
		if (sorted) return;
	}
}
