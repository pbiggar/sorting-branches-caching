#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#define PREDICTOR_COUNT 64

#ifdef _USE_SOFTWARE_PREDICTOR
#include <stdio.h>
struct predictor
{
	unsigned long long taken_count[4]; /* # of branches rtaken */
	unsigned long long not_taken_count[4]; /* # of branches not taken */

	int state; /* the current state of the predictor */

	char description[200];
	/* 0 = saturated not-taken */
	/* 1 = unsaturated not-taken */
	/* 2 = unstaturated taken */
	/* 3 = saturated taken */

};

#define NOT_TAKEN_SATURATED 0
#define NOT_TAKEN 1
#define TAKEN 2
#define TAKEN_SATURATED 3

void init_predictor(struct predictor* pred);

void branch_taken(struct predictor* pred);
void branch_not_taken(struct predictor* pred);

void dump_predictor_info(FILE* file, struct predictor* pred, const char* description);
void describe_predictor(struct predictor* pred, const char*);


void add_predictor(struct predictor* predA, struct predictor* predB);

extern struct predictor global_predictor[PREDICTOR_COUNT];

#else

#define init_predictor(A) do{}while(0)
#define branch_taken(A) do{}while(0)
#define branch_not_taken(A) do{}while(0)
#define dump_predictor_info(A, B, C) do{}while(0)
#define describe_predictor(A, B) do{}while(0)
#define add_predictor(A, B) do{}while(0)


extern int global_predictor[];
#endif

#endif
