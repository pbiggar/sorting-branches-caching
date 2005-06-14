#include "cache_sorts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>

#define HEAP_SIZE 8 /* never set this to 1 */

/* these days we index from 0 - N-1
* the rightmost child is (parent + 1)*heapsize
* the parent = (child - 1) / heapsize 
*
* actually, we go from leftchild, which is (parent * heapsize) + 1
* parent = (child - 1) / heapsize
*/

/*
static int
check_heap_property(unsigned int a[], int N)
{
	int parent;
	int i;
	for(parent = 0; parent < N/HEAP_SIZE+1; parent++)
	{
		int l = parent*HEAP_SIZE + 1;
//		printf("checking children of %d (a[parent] = %d\n", parent, a[parent]);
		for(i = 0; i < HEAP_SIZE; i++)
		{
//			printf("checking child %d (a[%d] = %d)\n", HEAP_SIZE-i-1, r-i, a[r-i]);
			if (l+i >= N)
			{
//				printf("finsihing\n");
				break;
			}
			if (a[parent] > a[l+i])
			{
				printf("FAIL: node %d (%d) is greater than child %d (node %d (%d)) \n", parent, a[parent], i, l+i, a[l+i]);
				return -1;
			}
		}	
	}
	return 0;
}
*/


static void
fix_up(unsigned int heap[], int child)
{
	int parent;

	unsigned int v = heap[child];
	while(less(v, heap[parent = ((child-1)/HEAP_SIZE)]))
	{
		branch_taken(&global_predictor[10]);
		heap[child] = heap[parent];
		heap[parent] = v;
		child = parent;
	}
	branch_not_taken(&global_predictor[10]);
}

static void
fix_down(unsigned int heap[], int parent, int N)
{
	int child;
	int child2;
	unsigned int v = heap[parent];
/*	printf("fixdown when N = %d\n", N); */
	while(1) /* see if it has any children */
	{
		if (parent*HEAP_SIZE+1 < N) branch_taken(&global_predictor[0]);
		else
		{
			branch_not_taken(&global_predictor[0]);
			break;
		}
		child = parent*HEAP_SIZE + 1; /* this gets the rightmost node */
		child2 = child;

		if (less(heap[child2 + 0], heap[child]))
		{
			child = child2 + 0;
			branch_taken(&global_predictor[1]);
		}
		else branch_not_taken(&global_predictor[1]);
		if (less(heap[child2 + 1], heap[child]))
		{
			child = child2 + 1;
			branch_taken(&global_predictor[2]);
		}
		else branch_not_taken(&global_predictor[2]);
		if (less(heap[child2 + 2], heap[child]))
		{
			child = child2 + 2;
			branch_taken(&global_predictor[3]);
		}
		else branch_not_taken(&global_predictor[3]);
		if (less(heap[child2 + 3], heap[child]))
		{
			child = child2 + 3;
			branch_taken(&global_predictor[4]);
		}
		else branch_not_taken(&global_predictor[4]);

#if (HEAP_SIZE == 8)
		if (less(heap[child2 + 4], heap[child]))
		{
			child = child2 + 4;
			branch_taken(&global_predictor[5]);
		}
		else branch_not_taken(&global_predictor[5]);
		if (less(heap[child2 + 5], heap[child]))
		{
			child = child2 + 5;
			branch_taken(&global_predictor[6]);
		}
		else branch_not_taken(&global_predictor[6]);
		if (less(heap[child2 + 6], heap[child]))
		{
			child = child2 + 6;
			branch_taken(&global_predictor[7]);
		}
		else branch_not_taken(&global_predictor[7]);
		if (less(heap[child2 + 7], heap[child]))
		{
			child = child2 + 7;
			branch_taken(&global_predictor[8]);
		}
		else branch_not_taken(&global_predictor[8]);

#endif

		/* when the larger child isnt large enough to be promoted, stop */
		if (!less(heap[child], v))
		{
			branch_taken(&global_predictor[9]);
			break;
		}
		branch_not_taken(&global_predictor[9]);

		/* move down */
		heap[parent] = heap[child];
		parent = child;
	}
	heap[parent] = v;
}

void
cache_heapsort(unsigned int a[], int N)
{
	int start_padding, sentinel_count, size_to_allocate, i, length;
	unsigned int *heap ,*heap_data;

	describe_predictor(&global_predictor[0], "has children");
	describe_predictor(&global_predictor[1], "comparison 0");
	describe_predictor(&global_predictor[2], "comparison 1");
	describe_predictor(&global_predictor[3], "comparison 2");
	describe_predictor(&global_predictor[4], "comparison 3");
	describe_predictor(&global_predictor[5], "comparison 4");
	describe_predictor(&global_predictor[6], "comparison 5");
	describe_predictor(&global_predictor[7], "comparison 6");
	describe_predictor(&global_predictor[8], "comparison 7");
	describe_predictor(&global_predictor[9], "child promotion");
	describe_predictor(&global_predictor[10], "fix up");
	
	start_padding = HEAP_SIZE - 1;
	sentinel_count = (HEAP_SIZE) - (N % HEAP_SIZE) + 1;
	if (sentinel_count == HEAP_SIZE) sentinel_count = 0;

	/* the size is N + padding_at_the_start + padding_at_end, rounded up to next HEAPSIZE */
	size_to_allocate = ((N + start_padding + sentinel_count - 1) & ~(HEAP_SIZE-1)) + HEAP_SIZE;

	heap_data = memalign(HEAP_SIZE, size_to_allocate * sizeof(unsigned int));
	heap = &heap_data[start_padding]; /* verified */

	length = 0;


	/* first build the heap */
	/* actually, this takes up so little time that I'm not even going t othink about it */
	for(i = 0; i < N; i++) 
	{
		heap[length] = a[i];
		fix_up(heap, length);
		length++;
	}

	/* add sentinels for fixdown */
	for(i = 0; i < sentinel_count; i++)
	{
		heap[N + i] = UINT_MAX;
	}

	i = 0;

	/* this is unrolled to 8 which is entrirely independant of the heapsize. so there */
	/* now pick the smallest off it */
	while(length > 9) 
	{
		a[i+0] = heap[0];
		heap[0] = heap[length-1];
		heap[length-1] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+1] = heap[0];
		heap[0] = heap[length-2];
		heap[length-2] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+2] = heap[0];
		heap[0] = heap[length-3];
		heap[length-3] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+3] = heap[0];
		heap[0] = heap[length-4];
		heap[length-4] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+4] = heap[0];
		heap[0] = heap[length-5];
		heap[length-5] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+5] = heap[0];
		heap[0] = heap[length-6];
		heap[length-6] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+6] = heap[0];
		heap[0] = heap[length-7];
		heap[length-7] = UINT_MAX;
		fix_down(heap, 0, length);

		a[i+7] = heap[0];
		heap[0] = heap[length-8];
		heap[length-8] = UINT_MAX;
		fix_down(heap, 0, length);

		length -= 8;
		i += 8;
	}

	while(length > 1)
	{
		a[i++] = heap[0];
		heap[0] = heap[length-1];
		heap[length-1] = UINT_MAX;
		fix_down(heap, 0, --length);
	}

	a[i] = heap[0];

	free(heap_data);

}
