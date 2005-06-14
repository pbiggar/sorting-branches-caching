#include "branch_sorts.h"

#include <stdio.h>

#ifdef _USE_SOFTWARE_PREDICTOR
static struct predictor* up_down0;
static struct predictor* up_down1;
static struct predictor* up_down2;
#endif

static void
fixDown(unsigned int a[], int parent, int N)
{
	int child;
	unsigned int v = a[parent];
	while(1) /* see if it has any children */
	{
		if (2*parent <= N) branch_taken(up_down0);
		else
		{
			branch_not_taken(up_down0);
			break;
		}
		child = 2*parent;

		/* its important that this be less for my improvement */
		if (less(a[child], a[child+1])) /* see if it has more than 1 child - check which child is larger */
		{
			branch_taken(up_down1);
			child++;
		}
		else branch_not_taken(up_down1);

		if (!less(v, a[child])) /* when the larger child isnt large enough to be promoted, stop */
		{
			branch_taken(up_down2);
			break;
		}
		else branch_not_taken(up_down2);
	
		/* move down */ 
		a[parent] = a[child];

		parent = child;
	}
	
	a[parent] = v;

}

/*
static int
is_a_heap(unsigned int a[], int l, int r)
{
	int N = r-l+1;
	int k;
	for(k = 1; k <= N/2; k++)
	{
		int k_pos = k - 1;
		int l_pos = k * 2 - 1;
		int r_pos = k * 2;
		int l = l_pos + 1;
		int r = r_pos + 1;

		if (a[k_pos] < a[l_pos])
		{
			printf("FAIL: node %d (a[%d]=%d) is less than left child %d (a[%d]=%d)\n", k, k_pos, a[k_pos], l, l_pos, a[l_pos]);
			return 0;
		}
		if (a[k_pos] < a[r_pos])
		{
			printf("FAIL: node %d (a[%d]=%d) is less than right child %d (a[%d]=%d)\n", k, k_pos, a[k_pos], r, r_pos, a[r_pos]);
			return 0;
		}
	}
	return 1;
}*/


void
base_heapsort(unsigned int a[], int N)
{
	int k;

	int is_even = !(N & 1);

#ifdef _USE_SOFTWARE_PREDICTOR
	up_down0 = &global_predictor[0];
	up_down1 = &global_predictor[1];
	up_down2 = &global_predictor[2];
#endif

	/* we switch predictors after the creation, so that we can record both */
	describe_predictor(&global_predictor[0], "Creation, has children");
	describe_predictor(&global_predictor[1], "Creation, child comparison");
	describe_predictor(&global_predictor[2], "Creation, child promotion");
	describe_predictor(&global_predictor[3], "Destruction, has children");
	describe_predictor(&global_predictor[4], "Destruction, child comparison");
	describe_predictor(&global_predictor[5], "Destruction, child promotion");
	
	if (is_even)
	{
		/* dont use compexch so that its only used in quicksort and its easier
		 * to code the branch_taken/branch_not_taken code */
        if (a[(N-1)/2] < a[N-1])
		{
			unsigned int t = a[N-1];
			a[N-1] = a[(N-1)/2];
			a[(N-1)/2] = t;
		}
		N--;
	}

	/* construct the heap */ 
	for(k=N/2; k >= 1; k--)
	{
		fixDown(&a[-1], k, N); /* while pretending there are no parents with 1 child*/
	}

	if (is_even)
	{
		exch(a[0], a[N]); /* the final place */
		fixDown(&a[-1], 1, N); /* still pretending its not there */
	}

	/* destroy the heap for the sortdown */
#ifdef _USE_SOFTWARE_PREDICTOR
	up_down0 = &global_predictor[3];
	up_down0->state = global_predictor[0].state;
	up_down1 = &global_predictor[4];
	up_down1->state = global_predictor[1].state;
	up_down2 = &global_predictor[5];
	up_down2->state = global_predictor[2].state;
#endif
	/* we cant overwrite anything with anything from limits.h, because this is in place */
	while(N>8)
	{
		unsigned int temp = a[0]; /* this is the right child */
		a[0] = a[N-1]; /* move the value to be sifted down */
		fixDown(&a[-1], 1, N-1); /* here we pretend there are 2 children */
		a[N-1] = temp; /* write the high value in its place */

		/* now the left child */
		temp = a[0];
		a[0] = a[N-2];
		fixDown(&a[-1], 1, N-2); /* now it has no children, as in the original algorithm */
		a[N-2] = temp;

		/* unroll - this has little effect ~= .1 percent*/
		temp = a[0];
		a[0] = a[N-3];
		fixDown(&a[-1], 1, N-3);
		a[N-3] = temp;

		temp = a[0];
		a[0] = a[N-4];
		fixDown(&a[-1], 1, N-4);
		a[N-4] = temp;

		temp = a[0];
		a[0] = a[N-5];
		fixDown(&a[-1], 1, N-5);
		a[N-5] = temp;

		temp = a[0];
		a[0] = a[N-6];
		fixDown(&a[-1], 1, N-6);
		a[N-6] = temp;

		temp = a[0];
		a[0] = a[N-7];
		fixDown(&a[-1], 1, N-7);
		a[N-7] = temp;

		temp = a[0];
		a[0] = a[N-8];
		fixDown(&a[-1], 1, N-8);
		a[N-8] = temp;


		N -= 8;
	}

	while(N>1)
	{
		unsigned int temp = a[0]; 
		N--;
		a[0] = a[N]; 
		fixDown(&a[-1], 1, N); 
		a[N] = temp; 
	}
}
