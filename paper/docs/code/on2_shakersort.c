void shakersort(Item a[], int N)
{
	int i, j, k = N-1, sorted;
	for (i = 0; i < k;  )
	{
		for (j = k; j > i; j--)
			if (less(a[j],a[j-1]))
				exch(a[j-1],a[j]);
		i++;
		sorted = 1;
		for (j = i ; j < k; j++)
		{
			if (less(a[j+1],a[j]))
			{
				exch(a[j],a[j+1]);
				sorted = 0;
			}
		}
		if (sorted) return;
		k--;
	}
}
