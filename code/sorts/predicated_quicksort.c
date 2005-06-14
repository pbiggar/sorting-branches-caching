#include <stdlib.h>
#include <stdio.h>

#include "base_sorts.h"

#define THRESHHOLD 10


static int* stack = NULL;
static int stack_index = 0;

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
push(int valueA, int valueB)
{
	stack[stack_index++] = valueA;
	stack[stack_index++] = valueB;
}

inline static int 
pop()
{
	stack_index--;
	return stack[stack_index];
}

inline static void
stackclear()
{
	if (stack != NULL) free(stack);
	stack = NULL;
}

inline static int
stackempty()
{
	return (stack_index==0);
}

static int
partition(unsigned int a[], int l, int r)
{
	unsigned int v = a[r];
	int i, j;

	unsigned int addr;
	unsigned int key = a[l];
	int cond;

	i = l + 1;
	j = r - i - 1;
	do 
	{
		cond = key < v;
		addr = i;
		i = i + cond;
		addr = addr + (j & (cond-1));
		exch(key, a[addr]);
//		asm
//		(
//			"xchgl	(%1, %2, 4), %0\n\t" // exch(key, a[addr])
//			:	"=r"	(key) // 0
//			:	"r"		(a) // 1
//			,	"r"		(addr) // 2
//		);

		j = j - 1;
	}
	while ( j > 0 );

	// i and j are the same, so just swap key with a[i]
	exch(key, a[i]);

	// but i needs to end up pointing to the smallest of the RHS of the partition
	cond = a[i] < v;
	i += cond;

	/* now put key and pivot in the right place */
	if ( key < v ) 
	{
		a[l] = key;
		a[r] = a[i];
		a[i] = v;
		return i;
	}
	else 
	{
		a[l] = a[i-1];
		a[i-1] = v;
		a[r] = key;
		return i-1;
	}
	asm("nop\n\t");
}

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
			a[j] = a[j-1];
			j--;
		}
		a[j] = v;
	}
}


/*static void
print_stack()
{
	printf("\nThe stack is: ");
	for(int i = 0; i < stack_index; i++)
	{
		printf(" %d, ", stack[i]);
	}
}
*/

void
predicated_quicksort(unsigned int a[], int N)
{
	int l, r;
	int i;
	int m;

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

		exch(a[m], a[r-1]);
		compexch(a[l], a[r-1]);
		compexch(a[l], a[r]);
		compexch(a[r-1], a[r]);

		i = partition(a,l+1,r-1);

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
