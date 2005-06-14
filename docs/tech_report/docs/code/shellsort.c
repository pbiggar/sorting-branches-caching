void improved_gonnet_shellsort(Item a[], int N)
{
	int i, h, min, bound; 

	/* shellsort it */
	for (h = 5*N/11; h >= 5; h = 5*h/11)
	{
		for (i = h; i <= N-1; i++)
		{
			int j = i; 
			Item v = a[i]; 
			while (j >= h && less(v, a[j-h]))
			{ 
				a[j] = a[j-h]; 
				j -= h; 
			}
			a[j] = v; 
		}
	}

	/* put a sentinel in place */
	min = 0;
	bound = 11;
	if (N < bound) bound = N;

	for(i = 1; i < bound; i++)
		if(less(a[i], a[min]))
			min = i;
	exch(a[0], a[min]);

	/* do the final insertion sort */
	for(i = 1; i < N; i++)
	{
		int j = i;
		Item v = a[i];

		while(less(v, a[j-1]))
		{
			a[j] = a[j-1];
			j--;
		}
		a[j] = v;
	}
}
