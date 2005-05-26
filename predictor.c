#include "predictor.h"

#ifdef _USE_SOFTWARE_PREDICTOR
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
	/*pred->state = NOT_TAKEN_SATURATED; */
	pred->state = TAKEN;
}

void
describe_predictor(struct predictor* pred, const char* description)
{
	strncpy(pred->description, description, 200);
}

void 
branch_taken(struct predictor* pred)
{
	int s = pred->state;
//	pred->branch_count[s]++;
	pred->taken_count[s]++;

	/* need to update the predictor? */
	if (s != TAKEN_SATURATED) pred->state++;
}

void 
branch_not_taken(struct predictor* pred)
{
	int s = pred->state;
//	pred->branch_count[s]++;
	pred->not_taken_count[s]++;

	/* need to update the predictor? */
	if (s != NOT_TAKEN_SATURATED) pred->state--;
}

void
add_predictor(struct predictor* predA, struct predictor* predB)
{
	predA->taken_count[0] += predB->taken_count[0];
	predA->taken_count[1] += predB->taken_count[1];
	predA->taken_count[2] += predB->taken_count[2];
	predA->taken_count[3] += predB->taken_count[3];
	predA->not_taken_count[0] += predB->not_taken_count[0];
	predA->not_taken_count[1] += predB->not_taken_count[1];
	predA->not_taken_count[2] += predB->not_taken_count[2];
	predA->not_taken_count[3] += predB->not_taken_count[3];
}

void
dump_predictor_info(FILE* file, struct predictor* pred, const char* description)
{
	fprintf(file,	"%s - %s:\n"
					"states                     %13s %13s %13s %13s %13s\n"
					"branch_count:              %13llu %13llu %13llu %13llu %13llu\n"
					"correct_count:             %13llu %13llu %13llu %13llu %13llu\n"
					"incorrect_count:           %13llu %13llu %13llu %13llu %13llu\n"
					"taken_count:               %13llu %13llu %13llu %13llu %13llu\n"
					"not_taken_count:           %13llu %13llu %13llu %13llu %13llu\n"
					"current state:             %13s\n\n",
					description, pred->description,
					state_names[0], state_names[1],
					state_names[2], state_names[3], "Total",
					// branch count
					pred->taken_count[0] + pred->not_taken_count[0],
					pred->taken_count[1] + pred->not_taken_count[1],
					pred->taken_count[2] + pred->not_taken_count[2],
					pred->taken_count[3] + pred->not_taken_count[3],
					pred->taken_count[0] + pred->not_taken_count[0] +
					pred->taken_count[1] + pred->not_taken_count[1] +
					pred->taken_count[2] + pred->not_taken_count[2] +
					pred->taken_count[3] + pred->not_taken_count[3],
					
					// correct
					pred->not_taken_count[0], pred->not_taken_count[1],
					pred->taken_count[2], pred->taken_count[3],
					pred->not_taken_count[0] + pred->not_taken_count[1] +
					pred->taken_count[2] + pred->taken_count[3],

					// incorrect
					pred->taken_count[0], pred->taken_count[1],
					pred->not_taken_count[2], pred->not_taken_count[3],
					pred->taken_count[0] + pred->taken_count[1] +
					pred->not_taken_count[2] + pred->not_taken_count[3],

					// taken
					pred->taken_count[0], pred->taken_count[1],
					pred->taken_count[2], pred->taken_count[3],
					pred->taken_count[0] + pred->taken_count[1] +
					pred->taken_count[2] + pred->taken_count[3],
					
					// not taken
					pred->not_taken_count[0], pred->not_taken_count[1],
					pred->not_taken_count[2], pred->not_taken_count[3],
					pred->not_taken_count[0] + pred->not_taken_count[1] +
					pred->not_taken_count[2] + pred->not_taken_count[3],

					state_names[pred->state]);
}



struct predictor global_predictor[PREDICTOR_COUNT];
#else

int global_predictor[0];

#endif
