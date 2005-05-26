#include <stdio.h>
#include <time.h>
#define exch(A, B) do { unsigned int t = (A); (A) = (B); (B) = t; } while(0)

int
higher_level_partition(unsigned int a[], int l, int r)
{
	unsigned int v = a[r];
	int i, j;

	unsigned int addr;
	unsigned int key = a[l];
	int cond;

	i = l + 1;
	j = r - i - 1;
	do 
	{
		cond = key < v;
		addr = i;
		i = i + cond;
		addr = addr + (j & (cond-1));
		exch(key, a[addr]);
		j = j - 1;
	}
	while ( j > 0 );

	// i and j are the same, so just swap key with a[i]
	exch(key, a[i]);

	// but i needs to end up pointing to the smallest of the RHS of the partition
	cond = a[i] < v;
	i += cond;


	/* now put key and pivot in the right place */
	if ( key < v ) 
	{
		a[l] = key;
		a[r] = a[i];
		a[i] = v;
		return i;
	}
	else 
	{
		a[l] = a[i-1];
		a[i-1] = v;
		a[r] = key;
		return i-1;
	}
}


int
partition(unsigned int a[], int l, int r)
{

	unsigned int pivot;
	int i, j;

	asm // source first, then destination, 
		// which is different from intel
	(
		// preamble
		"movl	(%%eax, %%edx, 4), %%esi\n\t" // pivot = a[r]
		"movl	(%%eax, %%ecx, 4), %%edi\n\t" // key = a[l]

		"incl	%%ecx\n\t" // i = l + 1
		"decl	%%edx\n\t" // j = r - 1

		// loop
		"\n.LOOP_LABEL%=:\n\t" // do {

		"movl 	%%edx, %%ebx\n\t" // addr = j
		"cmpl 	%%esi, %%edi\n\t" // if (cond) addr = i
		"cmovb 	%%ecx, %%ebx\n\t" 
		"xchgl	(%%eax, %%ebx, 4), %%edi\n\t" // exch(key, a[addr])

		"movl	$0, %%ebx\n\t" // inc = cond
		"setb	%%bl\n\t"

		"addl	%%ebx, %%ecx\n\t" // i += inc;

		"decl	%%edx\n\t" // j += (inc - 1)
		"addl	%%ebx, %%edx\n\t" 


		"cmpl	%%ecx, %%edx\n\t" // } while(j >= i)
		"jae	.LOOP_LABEL%=\n\t" 

		// postamble
		"incl	%%edx\n\t" // exch ( a[j+1],a[r])
		"xchgl	%%esi, (%%eax, %%edx, 4)\n\t" 

		:	"=c"	(i)
		,	"=d"	(j)
		,	"=S"	(pivot)

		:	"c"		(l)
		,	"d"		(r)
		,	"a"		(a)

		:	"%ebx", "%edi"

	);

	// the probelm here is that when the function becomes inlined, whats popping off the stack could be anything.
	// solution: label the registers as clobbered. then a new asm to transfer pivot into the right position

	asm
	(
		"movl	%%esi,(%%eax, %%ebx, 4)\n\t" 

		: // no output regs

		:	"b"	(r)
		,	"a"	(a)
		,	"S"	(pivot)
	);


	return j;
}


#define S 1000

int
main()
{
	int k;
	int i;
	unsigned int* a = (unsigned int*)malloc(S * sizeof(unsigned int));
	unsigned int* b = (unsigned int*)malloc(S*S* sizeof(unsigned int)); 
	// b keeps a track of how many times a number is set.
	//then we can check it at the end to see if its right
	unsigned int pivot;
	unsigned int v;

	for(i = 0; i < 1000000; i++)
	{
		printf("i is %d, ts = %ld\n", i, time(NULL));
		srand(i);
		memset(b, 0 , S*S*sizeof(unsigned int));

		// fill the array
		for(k = 0; k < S; k++)
		{
			a[k] = rand() % S;
			b[a[k]]++; // count the number of times this number is there
		}

		// debug
		for(k = 0; k < S; k++)
		{
//			printf("a[%d] = %d\n", k, a[k]);
		}

		pivot = higher_level_partition(a, 0, S-1);
		v = a[pivot];
//		printf("partitioned with %d at a[%d]\n", v, pivot);

		for(k = 0; k < S; k++)
		{
//			printf("a[%d] = %d\n", k, a[k]);
		}
		// check that its properly partitioned
		for(k = 0; k < pivot; k++)
		{
			b[a[k]]--;
			if (a[k] > v)
			{
				printf("(1. bad partition: i = %d, k = %d, a[k] = %d, pivot = %d\n", i, k, a[k], v);
				return -1;
			}
		}
		b[a[pivot]]--;
		for(k = pivot + 1 ; k < S; k++)
		{
			b[a[k]]--;
			if (a[k] < v)
			{
				printf("2. bad partition: i = %d, k = %d, a[k] = %d, pivot = %d\n", i, k, a[k], v);
				return -1;
			}
		}


		// check that the same numbers are in place
		for(k = 0; k < S*S; k++)
		{
			if (b[k] != 0)
			{
				printf("bad b (i == %d): k = %d, b[k] = %d\n", i, k, b[k]);
				return -1;
			}	
		}
	}

	return 0;
}


