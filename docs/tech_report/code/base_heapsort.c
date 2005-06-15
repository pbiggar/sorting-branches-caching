void fixDown(Item a[], int parent, int N)
{
	Item v = a[parent];
	while(2*parent <= N) /* check for children */
	{
		int child = 2*parent;

		/* find larger child */
		if (less(a[child], a[child+1])) child++; 

		/* stop when not larger than parent */
		if (!less(v, a[child])) break; 
	
		a[parent] = a[child];
		parent = child;
	}
	a[parent] = v;
}

void base_heapsort(Item a[], int N)
{
	/* construct the heap */ 
	for(int k=N/2; k >= 1; k--) fixDown(&a[-1], k, N);

	/* destroy the heap for the sortdown */
	while(N>1)
	{
		Item temp = a[0]; 
		a[0] = a[--N]; 
		fixDown(&a[-1], 1, N); 
		a[N] = temp; 
	}
}
