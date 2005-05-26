#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "cache_sorts.h"

#define THRESHHOLD 10

static int* stack = NULL;
static int stack_index = 0;

/* C is the number of keys that fit in the cache */
#define C IN_PLACE_LIMIT
#define C_DIV_3 (C / 3)
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

	if (N > 2 * C) /* make space for all the pivots being pushed */
	{
		lg2N +=  N / C_DIV_3;
	}

/*	printf("\nAllocating a stack %d ints big (lg2N == %d)\n", (lg2N + 2) << 2, lg2N); */

	/* +2 for initial values, *2 cuase we push 2 ints a time */
	/* we only really need about 3 less, cause of the threshhold, but this doesnt hurt */
	stack = malloc(((lg2N + 2) * 2) * sizeof(int));
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
	unsigned int v;
	int i, j;

	v = a[r];
	i = l - 1;
	j = r;

	for(;;)
	{
		while (less(a[++i], v))
			;
		while (less(v, a[--j]))
			;
		

		if (i >= j)
			break;

		exch(a[i], a[j]);
	}

	exch(a[i], a[r]);

	return i;
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

/* this is included so that we can replace the < N with <= N
   this saves us from adding N to everything we put into it */
static void
insertion_sentinel(unsigned int a[], int N)
{
	int i;
	unsigned min;
	min = 0;
	for(i = 1; i <= N; i++)
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
/*	printf("N = %d\n", N); */
	for(i = 1; i <= N; i++)
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

/* this is slightly under a page, to make room for extra allocated bits*/
/* glibc source indicates that 16 bytes are used for the allocation. */
/*2 words are used by the pointer and the count */
#define KEYS_PER_LIST 1018

struct List
{
	unsigned int blocks[KEYS_PER_LIST];
	struct List* previous;
	int count;
};

static struct List **lists = NULL;
static int list_count = 0;
static void* list_memory = NULL;
static struct List* next_available_list = NULL; 

static void
listinit(int number_of_pivots, int N)
{
	int i;
	long num_lists_required = number_of_pivots + (N / KEYS_PER_LIST);
	lists = malloc(sizeof(struct List*) * number_of_pivots);
	list_memory = malloc(sizeof(struct List) * num_lists_required);
	next_available_list = list_memory;

	for(i = 0; i < number_of_pivots; i++)
	{
		lists[i] = next_available_list++;
		lists[i]->count = 0;
		lists[i]->previous = lists[i];
	}
	list_count = number_of_pivots;
}

static void
listclear()
{
	if (lists == NULL)
		return;

	free(list_memory);
	free(lists);
	lists = NULL;
}

/* these functinos are not symetric. you fill it first. then you pop it. nothing more is required, or supported */
static inline void
add_to_list(unsigned int list_index, unsigned int key)
{
	struct List* list = lists[list_index];

/*	printf("adding key %d to list %d\n", key, list_index); */
	if (list->count == KEYS_PER_LIST) /* if we're full up */
	{
		struct List* old_list = list;
		list = next_available_list++;

		list->previous = old_list;

		list->count = 0;
		lists[list_index] = list;
	}

	list->blocks[list->count] = key;
	list->count++;

}

/* an empty list has count 0. that is all anybody needs to know */
static inline unsigned int 
pop_list(unsigned int list_index)
{
	struct List* list = lists[list_index];
	unsigned int result;

	list->count--;
	result = list->blocks[list->count];

	/* if we've run on */
	if (list->count == 0)
	{
		list = list->previous;
		if (list != NULL)
		{
			lists[list_index] = list;
		}
	}

/*	printf("popping key %d from list %d\n", result, list_index); */

	return result;
}



void
multi_quicksort(unsigned int a[], int N)
{
	int l, r;
	int i, j, k;
	int m;
	int pivot_count = 0;
	int min;
	unsigned int* pivots = NULL;
	unsigned int* pivots_memory = NULL;

	unsigned int v;
	int temp;

	stackinit(N);
	/* do a multi-quicksort */
	if (N > 2 * C)
	{
		/* this number is not accurate. but it is consistant, and so removes that bug */
		/* the -1 removes a bug where N is an exact number of C_DIV_3s */
		pivot_count = (N-1) / C_DIV_3;

		/* allocateenough emmory for sentinels at either side*/
		pivots_memory = malloc(sizeof(unsigned int) * (pivot_count + 2));
		pivots = &pivots_memory[1];

		listinit(pivot_count + 1, N);

		/* choose the pivots */
		pivots[-1] = 0;
		for(i = 0; i < pivot_count; i++)
		{
			pivots[i] = a[(i+1) * C_DIV_3];
		}
		pivots[pivot_count] = UINT_MAX;
		
		/* sort the pivots - theres already sentinels */
		insertion(pivots, pivot_count-1);

		/* fill the lists, one pivot group at a time */
		for(i = 0; i < N; i++)
		{
			if ((i % (C_DIV_3) == 0) && (i != 0))
			{
				continue;
			}

			v = a[i];
			l = 0;
			r = pivot_count+1;
			
			while(1)
			{
				k = (l+r) >> 1;

				if (v > pivots_memory[k+1])
				{
					l = k+1;
					continue;
				}
				
				if (v < pivots_memory[k])
				{
					r = k;
					continue;
				}

				add_to_list(k, v);
				break;
			}
		}
/*		printf("count = %d\n", count); */

		/* restore them in place */
		j = 0;

		/* first we get the smallest from the first set. then we dont ahve to do sentinels at all */
		min = 0;
		temp = 0;
		v = UINT_MAX;
		while(lists[0]->count > 0)
		{
			a[j] = pop_list(0);
			if (a[j] < v) /* find the smallest item */
			{
				min = j;
				v = a[j];
			}
			j++;
		}
		a[j++] = pivots[0];
		if (a[j] < a[min])
			min = j;

		exch(a[0], a[min]);

		/* the pivot is in its final place */
		push(j-2,temp);
		temp = j;

		for(i = 1; i < pivot_count; i++)
		{
			while(lists[i]->count > 0)
			{
				a[j++] = pop_list(i);
			}
			a[j++] = pivots[i];

			/* the pivot is in its final place */
			push(j-2,temp);
			temp = j;
		}

		if (temp < N-1)
			push(N-1,temp);

		/* add the ones to the right of the last pivot */
		while(lists[i]->count > 0)
		{
			a[j++] = pop_list(i);
		}
		free(pivots_memory);
		l = pop();
		r = pop();
	}
	else
	{
		r = N-1;
		l = 0;
	}

	while(1)
	{
		if (r - l <= THRESHHOLD)
		{
			if (l == 0)
			{
				insertion_sentinel(&a[0], r);
			}
			insertion(&a[l], r-l);
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
	listclear();
}
