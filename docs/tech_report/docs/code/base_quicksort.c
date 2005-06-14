#define THRESHHOLD 10

static int* stack = NULL;
static int stack_index = 0;

static void
stackinit(unsigned int N)
{

	int lg2N = 32; /* im hard coding this and i dont care */
	unsigned int msbN = 1 << (lg2N - 1);
	while(msbN > N)
	{
		lg2N--;
		msbN = msbN >> 1;
	}
	stack = malloc(((lg2N + 2) << 2) * sizeof(int));
}

inline static void push(int valueA, int valueB)
{
	stack[stack_index++] = valueA;
	stack[stack_index++] = valueB;
}

inline static int pop()
{
	stack_index--;
	return stack[stack_index];
}

inline static void stackclear()
{
	if (stack != NULL) free(stack);
	stack = NULL;
}

inline static int stackempty() { return (stack_index==0); }

static int partition(Item a[], int l, int r)
{

	Item v = a[r];
	int i = l - 1;
	int j = r;

	for(;;)
	{
		while (less(a[++i], v))
			;
		while (less(v, a[--j]))
			;
		
		if (i >= j) break;
		exch(a[i], a[j]);
	}
	exch(a[i], a[r]);

	return i;
}

base_quicksort(unsigned int a[], int N)
{
	stackinit(N);
	int r = N-1;
	int l = 0;

	while(1)
	{
		if (r - l <= THRESHHOLD)
		{
			if (stackempty()) break;

			l = pop(); r = pop();
			continue;
		}

		/* Median of 3 partitioning*/
		m = (l+r)/2;

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
	/* the +1 isnt immediately obvious. 
	 * its because THRESHHOLD is the difference between l and r up above */
	if (THRESHHOLD + 1 > N) insertion_sentinel(a,N);
	else insertion_sentinel(a,THRESHHOLD+1);
		
	insertion(a, N);
}
