static int* stack = NULL;
static int stack_index = 0;

inline static void
push(int valueA, int valueB)
{
	stack[stack_index++] = valueA;
	stack[stack_index++] = valueB;
}

inline static int 
pop()
{
	stack_index--;
	return stack[stack_index];
}

inline static void
stackclear()
{
	if (stack != NULL) free(stack);
	stack_index = 0;
	stack = NULL;
}

inline static int
stackempty()
{
	return (stack_index==0);
}

/*static void
print_stack()
{
	printf("\nThe stack is: ");
	for(int i = 0; i < stack_index; i++)
	{
		printf(" %d, ", stack[i]);
	}
}
*/

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

#if 0
static int
partition(unsigned int a[], int l, int r)
{

	unsigned int pivot;
	int i, j;

	asm /* source first, then destination,  */
		/* which is different from intel */
	(
		/* preamble */
		"movl	(%%eax, %%edx, 4), %%esi\n\t" /* pivot = a[r] */
		"movl	(%%eax, %%ecx, 4), %%edi\n\t" /* key = a[l] */

		"incl	%%ecx\n\t" /* i = l + 1 */
		"decl	%%edx\n\t" /* j = r - 1 */

		/* loop */
		"\n.LOOP_LABEL%=:\n\t" /* do { */

		"movl 	%%edx, %%ebx\n\t" /* addr = j */
		"cmpl 	%%esi, %%edi\n\t" /* if (cond) addr = i */
		"cmovb 	%%ecx, %%ebx\n\t" 
		"xchgl	(%%eax, %%ebx, 4), %%edi\n\t" /* exch(key, a[addr]) */

		"movl	$0, %%ebx\n\t" /* inc = cond */
		"setb	%%bl\n\t"

		"addl	%%ebx, %%ecx\n\t" /* i += inc; */

		"decl	%%edx\n\t" /* j += (inc - 1) */
		"addl	%%ebx, %%edx\n\t" 


		"cmpl	%%ecx, %%edx\n\t" /* } while(j >= i) */
		"jae	.LOOP_LABEL%=\n\t" 

		/* postamble */
		"incl	%%edx\n\t" /* exch ( a[j+1],a[r]) */
		"xchgl	%%esi, (%%eax, %%edx, 4)\n\t" 

		:	"=c"	(i)
		,	"=d"	(j)
		,	"=S"	(pivot)

		:	"c"		(l)
		,	"d"		(r)
		,	"a"		(a)

		:	"%ebx", "%edi"

	);

	/* the probelm here is that when the function becomes inlined, whats popping off the stack could be anything. */
	/* solution: label the registers as clobbered. then a new asm to transfer pivot into the right position */

	asm
	(
		"movl	%%esi,(%%eax, %%ebx, 4)\n\t" 

		: /* no output regs */

		:	"b"	(r)
		,	"a"	(a)
		,	"S"	(pivot)
	);


	return j;
}

#endif


