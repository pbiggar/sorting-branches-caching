#include "insertion.c"

void
on2_insertsort(unsigned int a[], int N)
{
	insertion_sentinel(a, N);
	insertion(a, N);
}
