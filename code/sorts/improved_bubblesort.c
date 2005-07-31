#include "on2_sorts.h"

void
bubblesort2(unsigned int a[], int N)
{
	int i = 0, j, k = 0;

#ifdef _USE_SOFTWARE_PREDICTOR
	char string[10];
#endif

	while(i < N-1)
	{
#ifdef _USE_SOFTWARE_PREDICTOR
		sprintf(string, "%d", i);
		describe_predictor(&global_predictor[i], string);
#endif
		k = N-1;
		for (j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				branch_taken(&global_predictor[i]);
				k = j;
				exch(a[j-1],a[j]);
			}
			else
			{
				branch_not_taken(&global_predictor[i]);
			}
		}
		i = k;
	}
}
