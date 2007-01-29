#include "../auxilliary/utils.h"
	
static void
insertion_sentinel(unsigned int a[], int N)
{
	int i;
	unsigned min;

	min = 0;
	for(i = 1; i < N; i++)
	{
		if(less(a[i], a[min]))
		{
			branch_taken(&global_predictor[1]);
			min = i;
		}
		else
		{
			branch_not_taken(&global_predictor[1]);
		}
	}
	exch(a[0], a[min]);

}

static void
insertion(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = 1; i < N; i++)
	{
		int j = i;
		v = a[i];

		while(less(v, a[j-1]))
		{
			branch_taken(&global_predictor[0]);
			a[j] = a[j-1];
			j--;
		}
		branch_not_taken(&global_predictor[0]);
		a[j] = v;
	}
}
