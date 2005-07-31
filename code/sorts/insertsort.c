#include "insertion.c"

void
on2_insertsort(unsigned int a[], int N)
{
	describe_predictor(&global_predictor[0], "Comparator");
	describe_predictor(&global_predictor[1], "Sentinel comparator");
	insertion_sentinel(a, N);
	insertion(a, N);
}
