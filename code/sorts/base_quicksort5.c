#include <stdlib.h>
#include <stdio.h>

#include "base_sorts.h"
#include "quick.c"

#define THRESHHOLD 14


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

inline static void
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


inline static int
med3(unsigned int array[], int l, int m, int r)
{
	unsigned int a = array[l];
	unsigned int b = array[m];
	unsigned int c = array[r];
	if (a < b)
	{
		branch_taken(&global_predictor[4]);
		if (b < c) 
		{
			branch_taken(&global_predictor[5]);
			return m;
		}
		else
		{
			branch_not_taken(&global_predictor[5]);
			if (a < c)
			{
				branch_taken(&global_predictor[6]);
				return r;
			}
			else
			{
				branch_not_taken(&global_predictor[6]);
				return l;
			}
		}
	}
	else
	{
		branch_not_taken(&global_predictor[4]);
		if (c < b)
		{
			branch_taken(&global_predictor[7]);
			return m;
		}
		else
		{
			branch_not_taken(&global_predictor[7]);
			if (c < a)
			{
				branch_taken(&global_predictor[8]);
				return r;
			}
			else
			{
				branch_not_taken(&global_predictor[8]);
				return l;
			}
		}
	}
}

/* this is bentley and mcilroys version */
/*static char *med3(char *a, char *b, char *c, int (*cmp)())
{ return cmp(a, b) < 0 ?
(cmp(b, c) < 0 ? b : cmp(a, c) < 0 ? c : a)
: (cmp(b, c) > 0 ? b : cmp(a, c) > 0 ? c : a);
*/

void
base_quicksort5(unsigned int a[], int N)
{
	int l, r;
	int i;
	int m;
	int im; /* names follow pl, pm, and pn from bently/mcilroy. used ir instead of in */

	stackinit(N);

	describe_predictor(&global_predictor[0], "i");
	describe_predictor(&global_predictor[1], "j");
	describe_predictor(&global_predictor[2], "partition end");
	describe_predictor(&global_predictor[3], "insertion");
	describe_predictor(&global_predictor[4], "median");
/*	describe_predictor(&global_predictor[4], "median of 5 ab"); */
	describe_predictor(&global_predictor[5], "median of 5 bc");
	describe_predictor(&global_predictor[6], "median of 5 ac");
	describe_predictor(&global_predictor[7], "median of 5 cb");
	describe_predictor(&global_predictor[8], "median of 5 ca");
	describe_predictor(&global_predictor[9], "median of 3 cmp1");
	describe_predictor(&global_predictor[10], "median of 3 cmp2");
	describe_predictor(&global_predictor[11], "median of 3 cmp3");

	r = N-1;
	l = 0;

	while(1)
	{
		int n = r - l;
		int n4 = n/4;
		if (r - l <= THRESHHOLD)
		{
			if (stackempty())
				break;

			l = pop();
			r = pop();
			continue;
		}
		
		/* pseudo-median of 5  - median of 1st last and meidna of middle 3*/
		m = (l+r)/2;
		if (n > 40)
		{
			im = med3(a, l + n4, m, m + n4);
			exch(a[im], a[r-1]);
		}

		
		pred_compexch(a[l], a[r-1], 9);
		pred_compexch(a[l], a[r], 10);
		pred_compexch(a[r-1], a[r], 11);

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

	/* add the predictors up */
	add_predictor(&global_predictor[4], &global_predictor[5]);
	add_predictor(&global_predictor[4], &global_predictor[6]);
	add_predictor(&global_predictor[4], &global_predictor[7]);
	add_predictor(&global_predictor[4], &global_predictor[8]);
	add_predictor(&global_predictor[4], &global_predictor[9]);
	add_predictor(&global_predictor[4], &global_predictor[10]);
	add_predictor(&global_predictor[4], &global_predictor[11]);
	init_predictor(&global_predictor[5]);
	init_predictor(&global_predictor[6]);
	init_predictor(&global_predictor[7]);
	init_predictor(&global_predictor[8]);
	init_predictor(&global_predictor[9]);
	init_predictor(&global_predictor[10]);
	init_predictor(&global_predictor[11]);
}
