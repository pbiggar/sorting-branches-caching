#include "on2_sorts.h"

void old_shellsort(unsigned int a[], int N)
{
	int l = 0;
	int r = N-1;
	int i, h; 

	describe_predictor(&global_predictor[0], "inner loop");
	for (h = r-l+1; h > 0; h = ((h > 1) && (h < 5)) ? 1 : 5*h/11)
	{
		for (i = l+h; i <= r; i++)
		{
			int j = i; 
			unsigned int v = a[i]; 
			while (j >= l+h && less(v, a[j-h]))
			{ 
				branch_taken(&global_predictor[0]);
				a[j] = a[j-h]; 
				j -= h; 
			}
			branch_not_taken(&global_predictor[0]);
			a[j] = v; 
		}
	}
}
