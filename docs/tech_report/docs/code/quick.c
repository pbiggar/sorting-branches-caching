static int
partition(unsigned int a[], int l, int r)
{
	unsigned int v;
	int i, j;

	v = a[r];
	i = l - 1;
	j = r;

	for(;;)
	{
		while (less(a[++i], v))
		{
			branch_taken(&global_predictor[0]);
		}
		branch_not_taken(&global_predictor[0]);

		while (less(v, a[--j]))
		{
			branch_taken(&global_predictor[1]);
		}
		branch_not_taken(&global_predictor[1]);
		
		if (i >= j)
		{
			branch_taken(&global_predictor[2]);
			break;
		}
		branch_not_taken(&global_predictor[2]);

		exch(a[i], a[j]);
	}

	exch(a[i], a[r]);

	return i;
}

