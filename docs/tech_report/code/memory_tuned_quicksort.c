memory_tuned_quicksort(unsigned int a[], int N)
{
	stackinit(N);
	int r = N-1;
	int l = 0;

	while(1)
	{
		if (r - l <= THRESHHOLD)
		{
			if (l == 0) insertion_sentinel(a, r);

			insertion(&a[l], r-l);
			if (stackempty()) break;

			l = pop();
			r = pop();
			continue;
		}

		/* Median of 3 partitioning*/
		int m = (l+r)/2;

		exch(a[m], a[r-1]);
		compexch(a[l], a[r-1]);
		compexch(a[l], a[r]);
		compexch(a[r-1], a[r]);

		int i = partition(a,l+1,r-1);

		if (i-l > r-i)
		{
			push(i-1,l);
			l = i+1;
		}
		else
		{
			push(r,i+1);
			r = i-1;
		}
	}
	stackclear();
}
