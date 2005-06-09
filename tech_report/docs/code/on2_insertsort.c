void insertion(Item a[], int N)
{
	// get a sentinel
	int min = 0;
	for(int i = 1; i < N; i++)
		if(less(a[i], a[min])) min = i;
	exch(a[0], a[min]);

	// sort the array
	for(int i = 1; i < N; i++)
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
