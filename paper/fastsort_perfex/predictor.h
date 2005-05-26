#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#if _USE_SOFTWARE_PREDICTOR
#include <stdio.h>
struct predictor
{
	int branch_count[4]; /* # of branches */

	int correct_count[4]; /* # of correct predictions */
	int incorrect_count[4]; /* # of incorrect predictions */

	int taken_count[4]; /* # of branches rtaken */
	int not_taken_count[4]; /* # of branches not taken */

	int state; /* the current state of the predictor */
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

void dump_predictor_info(FILE* file, struct predictor* pred, char* name);


extern struct predictor global_predictor0;
extern struct predictor global_predictor1;
extern struct predictor global_predictor2;
extern struct predictor global_predictor3;
extern struct predictor global_predictor4;
extern struct predictor global_predictor5;
extern struct predictor global_predictor6;
extern struct predictor global_predictor7;
extern struct predictor global_predictor8;
extern struct predictor global_predictor9;
#else

#define init_predictor(A) do{}while(0)
#define branch_taken(A)
#define branch_not_taken(A)
#define dump_predictor_info(A, B, C) do{}while(0)

#endif

#endif
