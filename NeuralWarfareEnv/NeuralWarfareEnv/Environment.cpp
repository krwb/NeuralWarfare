#include "Environment.h"

Environment::StepResult::StepResult(Observation* observation, float reward, bool terminated, bool truncated) :
	observation(observation),
	reward(reward),
	terminated(terminated),
	truncated(truncated)
{

}

Environment::StepResult::~StepResult()
{
	delete observation;
}
