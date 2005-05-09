void base_mergesort(Item a[], int N)
{
	Item * aux = malloc(N * sizeof(Item));
	unsigned int* source = a; unsigned int* target = aux;

	/* pre-sort */
	for(int i = 0; i <= N; i+=8)
	{
		int j = i+1;
		if (less(a[i+2], a[j])) j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i], a[j]);

		j = i+2;
		if (less(a[i+3], a[j])) j = i+3;
		compexch(a[i+1], a[j]);

		compexch(a[i+2], a[i+3]);

		j = i+5;

		if (less(a[j], a[i+6])) j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+4]);

		j = i+6;
		if (less(a[j], a[i+7])) j = i+7;
		compexch(a[j], a[i+5]);

		compexch(a[i+7], a[i+6]);
	}

	/* now beging merging */
	int track = 0; int next_count = 8;
	int i = 0; int j = next_count - 1;
	int k = 0; int d = 1;

	while(1)
	{
		/* iterate through the left list */
		while(1)
		{
			if (source[i] >= source[j]) break;
			target[k] = source[i++];
			k += d;
		}

		/* iterate through the right list */
		while(1)
		{
			if (source[i] <= source[j]) break;
			target[k] = source[j--];
			k += d;
		}

		/* check if we're in the middle */
		if(i == j)
		{
			target[k] = source[i];
			i = track + next_count;

			/* check whether we have left the building */
			if (i >= N)
			{
				if (next_count >= N) break;

				Item* temp_pointer = source;
				source = target;
				target = temp_pointer;

				/* the next iteration will be oevr bigger lists */
				next_count <<= 1;
				i = 0;
				j = next_count - 1;
				if (j >= N) j = N-1;
				track = 0;
				k = 0;
				d = 1;
				continue;
			}

			j = i + next_count - 1;
			if (j >= N) j = N-1;

			track = i;

			/* setup k for the next one */
			if (d == 1) 
			{
				d = -1;
				k = j;
			}
			else
			{
				d = 1;
				k = i;
			}
		}
		else if (source[i] == source[j])
		{
			target[k] = source[i++];
			k += d;
		}
	}

	if (target != a)
		memcpy(a, target, N * sizeof(Item)); 

	free(aux);
}
