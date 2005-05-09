#include "predictor.h"

#include <string.h>
	
static char state_names[4][20] = 
{
	"NT saturated",
	"NT",
	"T",
	"T saturated"
};

void
init_predictor(struct predictor* pred)
{
	memset(pred, 0, sizeof(struct predictor));
	//pred->state = NOT_TAKEN_SATURATED;
	pred->state = TAKEN;
}

void 
branch_taken(struct predictor* pred)
{
	int s = pred->state;
	pred->branch_count[s]++;
	pred->taken_count[s]++;

	// check what was predicted
	if (pred->state >= TAKEN) pred->correct_count[s]++; 
	else pred->incorrect_count[s]++;

	// need to update the predictor?
	if (pred->state != TAKEN_SATURATED) pred->state++;
		
	pred->state_change_count_up[s]++;
}

void 
branch_not_taken(struct predictor* pred)
{
	int s = pred->state;
	pred->branch_count[s]++;
	pred->not_taken_count[s]++;

	// check what was predicted
	if (pred->state <= NOT_TAKEN) pred->correct_count[s]++; 
	else pred->incorrect_count[s]++;

	// need to update the predictor?
	if (pred->state != NOT_TAKEN_SATURATED) pred->state--;
		
	pred->state_change_count_down[s]++;
}

void
dump_predictor_info(FILE* file, struct predictor* pred)
{
	fprintf(file,	"states                     %13s %13s %13s %13s %13s\n"
					"branch_count:              %13d %13d %13d %13d %13d\n"
					"correct_count:             %13d %13d %13d %13d %13d\n"
					"incorrect_count:           %13d %13d %13d %13d %13d\n"
					"taken_count:               %13d %13d %13d %13d %13d\n"
					"not_taken_count:           %13d %13d %13d %13d %13d\n"
					"state_change_count_up:     %13d %13d %13d %13d %13d\n"
					"state_change_count_down:   %13d %13d %13d %13d %13d\n"
					"current state:             %13s\n\n",

					state_names[0], state_names[1],
					state_names[2], state_names[3], "Total",
					pred->branch_count[0], pred->branch_count[1],
					pred->branch_count[2], pred->branch_count[3],
					pred->branch_count[0] + pred->branch_count[1] +
					pred->branch_count[2] + pred->branch_count[3],
					pred->correct_count[0], pred->correct_count[1],
					pred->correct_count[2], pred->correct_count[3],
					pred->correct_count[0] + pred->correct_count[1] +
					pred->correct_count[2] + pred->correct_count[3],
					pred->incorrect_count[0], pred->incorrect_count[1],
					pred->incorrect_count[2], pred->incorrect_count[3],
					pred->incorrect_count[0] + pred->incorrect_count[1] +
					pred->incorrect_count[2] + pred->incorrect_count[3],
					pred->taken_count[0], pred->taken_count[1],
					pred->taken_count[2], pred->taken_count[3],
					pred->taken_count[0] + pred->taken_count[1] +
					pred->taken_count[2] + pred->taken_count[3],
					pred->not_taken_count[0], pred->not_taken_count[1],
					pred->not_taken_count[2], pred->not_taken_count[3],
					pred->not_taken_count[0] + pred->not_taken_count[1] +
					pred->not_taken_count[2] + pred->not_taken_count[3],
					pred->state_change_count_up[0],
					pred->state_change_count_up[1],
					pred->state_change_count_up[2],
					pred->state_change_count_up[3],
					pred->state_change_count_up[0]+
					pred->state_change_count_up[1]+
					pred->state_change_count_up[2]+
					pred->state_change_count_up[3],
					pred->state_change_count_down[0],
					pred->state_change_count_down[1],
					pred->state_change_count_down[2],
					pred->state_change_count_down[3],
					pred->state_change_count_down[0]+
					pred->state_change_count_down[1]+
					pred->state_change_count_down[2]+
					pred->state_change_count_down[3],
					state_names[pred->state]);
}
