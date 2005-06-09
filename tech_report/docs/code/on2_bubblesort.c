void bubblesort(Item a[], int N)
{
	for (int i = 0; i < N-1; i++)
	{
		int sorted = 1;
		for (int j = N-1; j > i; j--)
		{
			if (less(a[j],a[j-1]))
			{
				exch(a[j-1],a[j]);
				sorted = 0;
			}
		}
		if (sorted) return;
	}
}
