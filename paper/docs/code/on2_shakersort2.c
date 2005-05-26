void shakersort2(Item a[], int N)
{
	int i = 0, j, k = N-1, l = N-1;
	for ( ; i < k; )
	{
		for (j = k; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				exch(a[j-1],a[j]);
				l = j;
			}
		}
		i = l;

		for (j = i ; j < k; j++)
		{
			if (less(a[j+1],a[j]))
			{
				exch(a[j],a[j+1]);
				l = j;
			}
		}
		k = l;
	}
}
