#define HEAP_SIZE 8 
fix_up(Item heap[], int child)
{
	Item v = heap[child];
	while(less(v, heap[int parent = ((child-1)/HEAP_SIZE)]))
	{
		heap[child] = heap[parent];
		heap[parent] = v;
		child = parent;
	}
}

fix_down(Item heap[], int parent, int N)
{
	Item v = heap[parent];
	while(int parent*HEAP_SIZE+1 < N) 
	{
		int child = parent*HEAP_SIZE + 1;
		int child2 = child;

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
		/* stop when the larger child is les than the parent */
		if (!less(heap[child], v)) break;

		/* move down */
		heap[parent] = heap[child];
		parent = child;
	}
	heap[parent] = v;
}

void memory_tuned_heapsort(Item a[], int N)
{
	int start_padding = HEAP_SIZE - 1;
	int length = 0;
	int sentinel_count = (HEAP_SIZE) - (N % HEAP_SIZE) + 1;
	if (sentinel_count == HEAP_SIZE) sentinel_count = 0;

	/* the size is N + padding_at_the_start + padding_at_end, 
	 * rounded up to next HEAPSIZE 
	 * */
	int size_to_allocate = ((N + start_padding + sentinel_count - 1) 
			& ~(HEAP_SIZE-1)) + HEAP_SIZE;

	Item* heap_data = memalign(HEAP_SIZE, size_to_allocate * sizeof(Item));
	Item* heap = &heap_data[start_padding];

	/* first build the heap */
	for(i = 0; i < N; i++) 
	{
		heap[length] = a[i];
		fix_up(heap, length);
		length++;
	}

	/* add sentinels for fixdown */
	for(int i = 0; i < sentinel_count; i++) heap[N + i] = UINT_MAX;

	int i = 0;
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
