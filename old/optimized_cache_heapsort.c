#include "cache_sorts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define HEAP_SIZE 4 /* never set this to 1 */

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
/*	printf("fixup: %d\n ", child); */
/*	printf("next child = %d, parent = %d, heap[child] = %d, heap[parent] = %d\n", child, ((child)/HEAP_SIZE), heap[child], heap[((child)/HEAP_SIZE)]);*/
	while(less(heap[child], heap[parent = ((child-1)/HEAP_SIZE)]))
	{
/*		printf("child = %d, parent = %d, heap[child] = %d, heap[parent] = %d\n", child, parent, heap[child], heap[parent]);*/

		exch(heap[child], heap[parent]);
		child = parent;
/*		printf("next child = %d, parent = %d, heap[child] = %d, heap[parent] = %d\n", child, ((child)/HEAP_SIZE), heap[child], heap[((child)/HEAP_SIZE)]);*/
	}
/*	printf("------------------------------------------------------------------\n");*/
}

static void
fix_down(unsigned int heap[], int parent, int N)
{
	int child;
	int child2;
/*	printf("fixdown when N = %d\n", N); */
	while(parent*HEAP_SIZE+1 < N) /* see if it has any children */
	{
		child = parent*HEAP_SIZE + 1; /* this gets the rightmost node */
		child2 = child;

		if (less(heap[child2 + 0], heap[child])) child = child2 + 0;
		if (less(heap[child2 + 1], heap[child])) child = child2 + 1;
		if (less(heap[child2 + 2], heap[child])) child = child2 + 2;
		if (less(heap[child2 + 3], heap[child])) child = child2 + 3;

#if (HEAP_SIZE == 8)
		if (less(heap[child2 + 4], heap[child])) child = child2 + 4;
		if (less(heap[child2 + 5], heap[child])) child = child2 + 5;
		if (less(heap[child2 + 6], heap[child])) child = child2 + 6;
		if (less(heap[child2 + 7], heap[child])) child = child2 + 7;
#endif

		/* when the larger child isnt large enough to be promoted, stop */
		if (!less(heap[child], heap[parent])) break;

		/* move down */
		exch(heap[parent], heap[child]); 
		parent = child;
	}
}

void
cache_heapsort(unsigned int a[], int l, int r)
{
	int N = (r-l)+1;
	int start_padding = HEAP_SIZE - 1;
	int sentinel_count = (HEAP_SIZE) - (N % HEAP_SIZE);

	/* the size is N + padding_at_the_start + padding_at_end, rounded up to next HEAPSIZE */
	int size_to_allocate = ((N + start_padding + sentinel_count - 1) & ~(HEAP_SIZE-1)) + HEAP_SIZE;

	unsigned int* heap_data = malloc(size_to_allocate * sizeof(unsigned int));
	unsigned int* heap = &heap_data[start_padding];
	int i;
	unsigned int length = 0;

	printf("start: %d, N: %d, end: %d, sum: %d\n", start_padding, N, sentinel_count, size_to_allocate);

	/* add sentinels for fixdown */
	for(i = 0; i < sentinel_count; i++)
	{
/*		printf("adding a sentinel (%d) at position %d\n", UINT_MAX, N+i); */
		heap[N + i] = UINT_MAX;
	} 

	/* first build the heap */
	for(i = 0; i < N; i++)
	{
		heap[length] = a[i];
		fix_up(heap, length);
/*		if (check_heap_property(heap, length) == -1)*/
/*		{*/
/*			print_int_array(heap, length+1, "fixing up", 0, length, i);*/
/*			exit(1);*/
/*		}*/
		length++;
	}

	i = 0;
	/* now pick the smallest off it */
	while(length > 7) /* this is unrolled to 8 which is entrirely independant of the heapsize. so there */
	{
		a[i+0] = heap[0];
		heap[0] = heap[length-1];
		heap[length-1] = UINT_MAX;
		fix_down(heap, 0, length-0);

		a[i+1] = heap[0];
		heap[0] = heap[length-2];
		heap[length-2] = UINT_MAX;
		fix_down(heap, 0, length-1);

		a[i+2] = heap[0];
		heap[0] = heap[length-3];
		heap[length-3] = UINT_MAX;
		fix_down(heap, 0, length-2);

		a[i+3] = heap[0];
		heap[0] = heap[length-4];
		heap[length-4] = UINT_MAX;
		fix_down(heap, 0, length-3);

		a[i+4] = heap[0];
		heap[0] = heap[length-5];
		heap[length-5] = UINT_MAX;
		fix_down(heap, 0, length-4);

		a[i+5] = heap[0];
		heap[0] = heap[length-6];
		heap[length-6] = UINT_MAX;
		fix_down(heap, 0, length-5);

		a[i+6] = heap[0];
		heap[0] = heap[length-7];
		heap[length-7] = UINT_MAX;
		fix_down(heap, 0, length-6);

		a[i+7] = heap[0];
		heap[0] = heap[length-8];
		heap[length-8] = UINT_MAX;
		fix_down(heap, 0, length-7);

		length -= 8;
		i += 8;

	}

	while(length > 0)
	{
		a[i++] = heap[0];
		heap[0] = heap[length-1];
		heap[length-1] = UINT_MAX;
		fix_down(heap, 0, length);
		length--;
	}

	free(heap_data);
}
