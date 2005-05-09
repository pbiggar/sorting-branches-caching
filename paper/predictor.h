#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include <stdio.h>
struct predictor
{
	int branch_count[4]; // # of branches

	int correct_count[4]; // # of correct predictions
	int incorrect_count[4]; // # of incorrect predictions

	int taken_count[4]; // # of branches rtaken
	int not_taken_count[4]; // # of branches not taken

	int state; // the current state of the predictor
	// 0 = saturated not-taken
	// 1 = unsaturated not-taken
	// 2 = unstaturated taken
	// 3 = saturated taken
	int state_change_count_up[4]; // the number of times the state changed
	int state_change_count_down[4]; // the number of times the state changed

};

#define NOT_TAKEN_SATURATED 0
#define NOT_TAKEN 1
#define TAKEN 2
#define TAKEN_SATURATED 3

void init_predictor(struct predictor* pred);

void branch_taken(struct predictor* pred);
void branch_not_taken(struct predictor* pred);

void dump_predictor_info(FILE* file, struct predictor* pred);

#endif
