#include "Environment.h"

Environment::StepResult::StepResult(Observation* observation, float reward, bool terminated, bool truncated, size_t ID) :
	observation(observation),
	reward(reward),
	terminated(terminated),
	truncated(truncated),
	ID(ID)
{

}

Environment::StepResult::~StepResult()
{
	delete observation;
}

Environment::Observation::Observation()
{
}

Environment::Observation::~Observation()
{
}

Environment::Environment()
{
}

Environment::~Environment()
{
}

Environment::Action::Action(StepResult* sr, size_t action) :
	ID(sr->ID),
	action(action)
{
}

Environment::Action::Action(size_t ID, size_t action) :
	ID(ID),
	action(action)
{
}

Environment::Action::~Action()
{
}
