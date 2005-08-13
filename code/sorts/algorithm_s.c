#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "base_sorts.h"

void 
algorithm_s(unsigned int a[], int N)
{
	unsigned int* aux = malloc(N * sizeof(unsigned int));
	int s; /* s keeps track of the target, we can write something to remove this */
	int i,j,k,l;
	int d; /* direction counter */
	
	int temp;

	int p, q, r; /* p keeps count of the number of items in this merge, q and r are counters for ascending and descending */
	unsigned int* source;
	unsigned int* target;

	describe_predictor(&global_predictor[0], "S2");
	describe_predictor(&global_predictor[1], "S3");
	describe_predictor(&global_predictor[2], "S5");
	describe_predictor(&global_predictor[3], "S6");
	describe_predictor(&global_predictor[4], "S7");
	describe_predictor(&global_predictor[5], "S8");
	describe_predictor(&global_predictor[6], "S10");
	describe_predictor(&global_predictor[7], "S11");
	describe_predictor(&global_predictor[8], "S12");
	describe_predictor(&global_predictor[9], "S13");
/*S1:*/
	s = 0; /* we should be able to elimnate s from here */
	p = 1;

S2:
	i = 0;
	j = N-1;
	k = -1;
	l = N;
	d = 1;
	q = p;
	r = p;

	if (s == 0)
	{
		branch_taken(&global_predictor[0]);
		source = a;
		target = aux;
	}
	else
	{
		branch_not_taken(&global_predictor[0]);
		source = aux;
		target = a;
	}

S3:
	if (source[i] > source[j])
	{
		branch_taken(&global_predictor[1]);
		goto S8;
	}
	else branch_not_taken(&global_predictor[1]);

/*S4:*/
	k = k + d;
	target[k] = source[i];

/*S5:*/
	i = i+1;
	q = q-1;
	if (q > 0)
	{
		branch_taken(&global_predictor[2]);
		goto S3;
	}
	else branch_not_taken(&global_predictor[2]);

S6:
	k = k+d;
	if (k == l)
	{
		branch_taken(&global_predictor[3]);
		goto S13;
	}
	else branch_not_taken(&global_predictor[3]);

	target[k] = source[j];

/*S7: */
	j = j-1;
	r = r-1;
	if (r>0)
	{
		branch_taken(&global_predictor[4]);
		goto S6;
	}
	else
	{
		branch_not_taken(&global_predictor[4]);
		goto S12;
	}
	

S8:
	k = k + d;
	target[k] = source[j];

/*S9:*/
	j = j-1;
	r = r-1;
	if (r > 0)
	{
		branch_taken(&global_predictor[5]);
		goto S3;
	}
	else branch_not_taken(&global_predictor[5]);

S10:
	k = k+d;
	if (k == l)
	{
		branch_taken(&global_predictor[6]);
		goto S13;
	}
	else
	{
		branch_not_taken(&global_predictor[6]);
		target[k] = source[i];
	}

/*S11: */
	i = i+1;
	q = q-1;
	if(q > 0)
	{
		branch_taken(&global_predictor[7]);
		goto S10;
	}
	branch_not_taken(&global_predictor[7]);

S12:
	q = p;
	r = p;
	d = -d;
	temp = k; /* swap */
	k = l;
	l = temp;
	if (j-i<p)
	{
		branch_taken(&global_predictor[8]);
		goto S10;
	}
	else
	{
		branch_not_taken(&global_predictor[8]);
		goto S3;
	}

S13:
	p = p+p;
	if (p < N)
	{
		branch_taken(&global_predictor[9]);
		s = 1-s;
		goto S2;
	}
	else branch_not_taken(&global_predictor[9]);

	if (s==0)
	{
		memcpy(a, target, N * sizeof(unsigned int)); 
	}

	free(aux);

	/* clear uninteresting predictors */
	init_predictor(&global_predictor[0]);
	init_predictor(&global_predictor[3]);
	init_predictor(&global_predictor[4]);
	init_predictor(&global_predictor[6]);
	init_predictor(&global_predictor[7]);
	init_predictor(&global_predictor[8]);
	init_predictor(&global_predictor[9]);

}
