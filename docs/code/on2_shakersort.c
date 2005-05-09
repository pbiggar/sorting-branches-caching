void shakersort(Item a[], int N)
{
	int k = N-1;
	for (int i = 0; i < k; )
	{
		// forwards
		for (int j = k; j > i; j--)
			if (less(a[j], a[j-1])) exch(a[j], a[j-1]);
		i++;

		// backwards
		for (int j = i ; j < k; j++)
			if (less(a[j+1], a[j])) exch(a[j+1], a[j]);
		k--;
	}
}
