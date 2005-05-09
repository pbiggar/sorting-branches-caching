void quicksort(Item a[], int l, int r)
{
	if (r <= l) return;
	int i = partition(a, l, r);
	quicksort(a, l, i-1);
	quicksort(a, i+1, r);
}

