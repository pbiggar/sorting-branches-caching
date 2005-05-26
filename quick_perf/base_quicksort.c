#include <stdlib.h>
#include <stdio.h>

#include "base_sorts.h"
#include "quick.c"

#define THRESHHOLD 13 


static void
stackinit(unsigned int N)
{

	int lg2N = 32; /* im hard coding this and i dont care */
	unsigned int msbN = 1 << (lg2N - 1);
	while(msbN > N)
	{
		lg2N--;
		msbN = msbN >> 1;
	}

/*	printf("\nAllocating a stack %d ints big (lg2N == %d)\n", (lg2N + 2) << 2, lg2N); */
	
	stack = malloc(((lg2N + 2) << 2) * sizeof(int));
}

inline static void
insertion_sentinel(unsigned int a[], int N)
{
	int i;
	unsigned min;

	min = 0;
	for(i = 1; i < N; i++)
	{
		if(less(a[i], a[min]))
		{
			min = i;
		}
	}
	exch(a[0], a[min]);

}

static void
insertion(unsigned int a[], int N)
{
	int i;
	unsigned int v;
	for(i = 2; i < N; i++)
	{
		int j = i;
		v = a[i];

		while(less(v, a[j-1]))
		{
			branch_taken(&global_predictor[3]);
			a[j] = a[j-1];
			j--;
		}
		branch_not_taken(&global_predictor[3]);
		a[j] = v;
	}
}


void
base_quicksort(unsigned int a[], int N)
{
	int l, r;
	int i;
	int m;

	describe_predictor(&global_predictor[0], "i");
	describe_predictor(&global_predictor[1], "j");
	describe_predictor(&global_predictor[2], "partition end");
	describe_predictor(&global_predictor[3], "insertion");

	stackinit(N);

	r = N-1;
	l = 0;

	while(1)
	{
		if (r - l <= THRESHHOLD)
		{
			if (stackempty())
				break;

			l = pop();
			r = pop();
			continue;
		}

		/* Median of 3 partitioning*/
		m = (l+r)/2;

		/* median of 3 exchanges */
		exch(a[m], a[r-1]);
		compexch(a[l], a[r-1]);
		compexch(a[l], a[r]);
		compexch(a[r-1], a[r]);

		i = partition(a,l+1,r-1);

		/* here is the bug */
		/* then key is being copied more times than necessary. the reason for this is that it is not being removed when it is taken as the key */
		/* instead, it is being put in place more than once */
		/* example: i == 1, j == 10; key = a[1]; key < pivot, so key is swapped with a[2], the key is now in a[1] and a[2]. uh oh  */
		if (i-l > r-i)
		{
			push(i-1,l);
			l = i+1;
		}
		else
		{
			push(r,i+1);
			r = i-1;
		}
	}

	stackclear();
	/* the +1 isnt immediately obvious. its because THRESHHOLD is the difference between l and r up above */
	if (2*THRESHHOLD > N) insertion_sentinel(a,N);
	else insertion_sentinel(a,2*THRESHHOLD);
		
	insertion(a, N);
}
