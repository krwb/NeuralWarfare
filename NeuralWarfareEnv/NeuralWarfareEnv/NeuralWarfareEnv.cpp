#include "NeuralWarfareEnv.h"

NeuralWarfareEnv::NeuralWarfareEnv(NeuralWarfareEngine& engine, size_t teamNum) : engine(engine)
{
	ConnectToTeam(teamNum);
}

NeuralWarfareEnv::~NeuralWarfareEnv()
{
}

void NeuralWarfareEnv::TakeAction(const std::vector<size_t>& actions)
{
	for (size_t i = 0; i < agents.size() && i < actions.size(); i++)
	{
		agents[i]->TakeAction(actions[i]);
	}
}

std::vector<Environment::StepResult> NeuralWarfareEnv::GetResult()
{
	StepResult sr(new NeuralWarfareEnv::MyObservation, 1, false, false);
	return std::vector<StepResult>();
}

std::vector<Environment::StepResult> NeuralWarfareEnv::Reset()
{
	return std::vector<StepResult>();
}

void NeuralWarfareEnv::ConnectToTeam(size_t teamId)
{
	NeuralWarfareEnv::teamId = teamId;
	for (NeuralWarfareEngine::Agent& agent : engine.agents)
	{
		if (agent.teamId == teamId)
		{
			agents.push_back(&agent);
		}
	}
}

Environment::Observation* NeuralWarfareEnv::getObservation(NeuralWarfareEngine::Agent* agent)
{
	MyObservation observation;

	return &observation;
}

std::vector<double> NeuralWarfareEnv::MyObservation::GetForNN()
{
	return std::vector<double>();
}
