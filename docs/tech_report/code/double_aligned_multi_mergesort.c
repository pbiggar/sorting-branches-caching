void double_aligned_multi_mergesort(Item a[], int N)
{
	/* assume that code is merged into cache sized segments at this point.
	 * This is now merged in one step, by the following code
	 */
	int K = (((N-1) >> (LIMIT_BITS))+1);

	int heap_length = 0;
	/* this changes to an even number (rounding up) */
	indices = memalign(HEAP_SIZE, ((K+1)^1) * sizeof(int)); 
	heap_data = memalign(HEAP_SIZE, (K+1) * HEAP_SIZE * sizeof(Item));
	heap = heap_data + (HEAP_SIZE - 1);
	memset(heap_data, 0xff, (K+1) * HEAP_SIZE * sizeof(Item));
	last_added_data = memalign(HEAP_SIZE, (K+1) * sizeof(Item));
	last_added_indices = memalign(HEAP_SIZE, (K) * sizeof(Item));

	last_added = &last_added_data[1];
	last_added_data[0] = 0;

	/* we add 8 from each to the Q */
	for(i = 0; i < K; i++) /* we do the last one ourselves */
	{
		/* this may cause k misses in the bimodal predictor. 
		 * but k is so small thats its not worth the effort */
		if (i & 1) /* this is a reverse list */
		{
			if (i == K-1) indices[i] = N-1; /* last mini-list */
			else indices[i] = ((i+1) << LIMIT_BITS) - 1;

			add_heap_line(&aux[indices[i]], heap, -1, heap_length, HEAP_SIZE);
			indices[i] -= HEAP_SIZE;

			last_added[i] = aux[indices[i]+1];
			last_added_indices[i] = i;

		}
		else /* foward list */
		{
			indices[i] = i << LIMIT_BITS;
			if (i == K-1)
			{
				int space_to_end = HEAP_SIZE;
				if (N - indices[i] < HEAP_SIZE ) space_to_end = N - indices[i];

				/* this makes a sort of sentinel ,which simplifies the logic below */
				indices[i+1] = N-1; 
				add_heap_line(&aux[indices[i]], heap, 1, heap_length, space_to_end); 
				indices[i] += space_to_end;
			}
			else
			{
				add_heap_line(&aux[indices[i]], heap, 1, heap_length, HEAP_SIZE);
				indices[i] += HEAP_SIZE;
			}

			last_added[i] = aux[indices[i]-1];
			last_added_indices[i] = i;
		}
		heap_length += HEAP_SIZE;
	}
	sort_last_index(last_added, last_added_indices, K);

	/* do the merge */
	i = 0;
	while(i < N)
	{
		a[i] = heap[0];
		heap[0] = heap[heap_length-1];
		heap[heap_length-1] = UINT_MAX;
		fix_down(heap, 0, --heap_length);

		if (a[i] == last_added[0])
		{
			int last_added_index = last_added_indices[0];

			if (last_added_index & 1)
			{
				/* we add 1 here because both indices are on an 
				 * item that hasnt yet been added */
				add_heap_line(&aux[indices[last_added_index]], 
						heap, -1, heap_length, HEAP_SIZE);
				indices[last_added_index] -= HEAP_SIZE;
				last_added[0] = aux[indices[last_added_index]+1];
				heap_length += HEAP_SIZE;
			}
			else
			{
				/* the will have a buddy on the right */
				add_heap_line(&aux[indices[last_added_index]], 
						heap, 1, heap_length, HEAP_SIZE);
				indices[last_added_index] += HEAP_SIZE;
				last_added[0] = aux[indices[last_added_index]-1];
				heap_length += HEAP_SIZE;
			}

			sort_last_index(last_added, last_added_indices, K);
		}
		i++;
	}
	free(indices); free(heap_data); free(last_added_data);free(last_added_indices);
}
