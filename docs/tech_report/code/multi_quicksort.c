#define KEYS_PER_LIST 1018
struct List
{
	unsigned int blocks[KEYS_PER_LIST];
	struct List* previous;
	int count;
};

static struct List **lists = NULL;
static int list_count = 0;
static void* list_memory = NULL;
static struct List* next_available_list = NULL; 

static void listinit(int number_of_pivots, int N)
{
	int i;
	long num_lists_required = number_of_pivots + (N / KEYS_PER_LIST);
	lists = malloc(sizeof(struct List*) * number_of_pivots);
	list_memory = malloc(sizeof(struct List) * num_lists_required);
	next_available_list = list_memory;

	for(i = 0; i < number_of_pivots; i++)
	{
		lists[i] = next_available_list++;
		lists[i]->count = 0;
		lists[i]->previous = lists[i];
	}
	list_count = number_of_pivots;
}

static void listclear()
{
	if (lists == NULL) return;

	free(list_memory);
	free(lists);
	lists = NULL;
}

/* these functinos are not symetric. you fill it first. 
 * then you pop it. nothing more is required, or supported */
static inline void
add_to_list(unsigned int list_index, unsigned int key)
{
	struct List* list = lists[list_index];

/*	printf("adding key %d to list %d\n", key, list_index); */
	if (list->count == KEYS_PER_LIST) /* if we're full up */
	{
		struct List* old_list = list;
		list = next_available_list++;

		list->previous = old_list;

		list->count = 0;
		lists[list_index] = list;
	}

	list->blocks[list->count] = key;
	list->count++;

}

/* an empty list has count 0. that is all anybody needs to know */
static inline unsigned int 
pop_list(unsigned int list_index)
{
	struct List* list = lists[list_index];
	unsigned int result;

	list->count--;
	result = list->blocks[list->count];

	/* if we've run on */
	if (list->count == 0)
	{
		list = list->previous;
		if (list != NULL) lists[list_index] = list;
	}

	return result;
}



void
multi_quicksort(unsigned int a[], int N)
{
	int pivot_count = 0;
	unsigned int* pivots = NULL;
	unsigned int* pivots_memory = NULL;

	stackinit(N);
	if (N > 2 * C)
	{
		/* this number is not accurate. 
		 * but it is consistant, and so removes that bug */
		/* the -1 removes a bug where N is an exact number of C_DIV_3s */
		int pivot_count = (N-1) / C_DIV_3;

		/* allocateenough emmory for sentinels at either side*/
		Item* pivots_memory = malloc(sizeof(Item) * (pivot_count + 2));
		Item* pivots = &pivots_memory[1];

		listinit(pivot_count + 1, N);

		/* choose the pivots */
		pivots[-1] = 0;
		for(i = 0; i < pivot_count; i++) pivots[i] = a[(i+1) * C_DIV_3];
		pivots[pivot_count] = UINT_MAX;
		
		/* sort the pivots - theres already sentinels */
		insertion(pivots, pivot_count-1);

		/* fill the lists, one pivot group at a time */
		for(i = 0; i < N; i++)
		{
			if ((i % (C_DIV_3) == 0) && (i != 0)) continue;

			Item *v = a[i];
			int l = 0;
			int r = pivot_count+1;
	
			/* binary search */
			while(1)
			{
				k = (l+r) >> 1;

				if (v > pivots_memory[k+1])
				{
					l = k+1;
					continue;
				}
				
				if (v < pivots_memory[k])
				{
					r = k;
					continue;
				}

				add_to_list(k, v);
				break;
			}
		}

		j = 0;

		min = 0;
		temp = 0;
		v = UINT_MAX;
		while(lists[0]->count > 0)
		{
			a[j] = pop_list(0);
			if (a[j] < v) /* find the smallest item */
			{
				min = j;
				v = a[j];
			}
			j++;
		}
		a[j++] = pivots[0];
		if (a[j] < a[min])
			min = j;

		exch(a[0], a[min]);

		/* the pivot is in its final place */
		push(j-2,temp);
		temp = j;

		for(i = 1; i < pivot_count; i++)
		{
			while(lists[i]->count > 0)
			{
				a[j++] = pop_list(i);
			}
			a[j++] = pivots[i];

			/* the pivot is in its final place */
			push(j-2,temp);
			temp = j;
		}

		if (temp < N-1)
			push(N-1,temp);

		/* add the ones to the right of the last pivot */
		while(lists[i]->count > 0)
		{
			a[j++] = pop_list(i);
		}
		free(pivots_memory);
		l = pop();
		r = pop();
	}
	else
	{
		r = N-1;
		l = 0;
	}

	/* as memory-tuned quicksort from here */
}
