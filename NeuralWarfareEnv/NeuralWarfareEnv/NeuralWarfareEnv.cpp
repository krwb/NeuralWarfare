#include "NeuralWarfareEnv.h"
#include "angleTools.h"
#include "KDTree.h"

NeuralWarfareEnv::NeuralWarfareEnv(NeuralWarfareEngine& engine, size_t teamNum) : engine(engine)
{
	ConnectToTeam(teamNum);
}

NeuralWarfareEnv::~NeuralWarfareEnv()
{

}

void NeuralWarfareEnv::TakeAction(const std::vector<Action>& actions)
{
	for (size_t i = 0; i < actions.size(); i++)
	{
		agents[actions[i].ID]->TakeAction(actions[i].action);
	}
}

std::vector<Environment::StepResult*> NeuralWarfareEnv::GetResult()
{
	std::vector<StepResult*> srts;
	for (size_t i = 0; i < agents.size(); i++)
	{
		srts.push_back(new StepResult(getObservation(agents[i]), 0, false, false,i));
	}
	return srts;
}

std::vector<Environment::StepResult*> NeuralWarfareEnv::Reset()
{
	return GetResult();
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

std::pair<float,double> getRelativePolarPos(const Vec2& origin, const Vec2& point, double originDirection = 0)
{
	std::pair<float, double> out;
	Vec2 relative = (origin - point);
	out.first = relative.Length();
	out.second = normalizeAngle(relative.Direction() - originDirection);
	return out;
}

Environment::Observation* NeuralWarfareEnv::getObservation(NeuralWarfareEngine::Agent* agent)
{
	MyObservation* observation = new MyObservation();

	std::vector<NeuralWarfareEngine::Agent*> hostileAgents =
		engine.kdTree.FindNearestNeighbors(agent->pos, 5,
			[agent](const NeuralWarfareEngine::Agent* a) {
				return a->teamId != agent->teamId && a->health > 0;
			}
		);
	std::vector<NeuralWarfareEngine::Agent*> friendlyAgents =
		engine.kdTree.FindNearestNeighbors(agent->pos, 5,
			[agent](const NeuralWarfareEngine::Agent* a) {
				return a->teamId == agent->teamId && a != agent && a->health > 0;
			}
		);

	observation->health = agent->health;

	for (NeuralWarfareEngine::Agent* hostileAgent : hostileAgents)
	{
		observation->hostileAgents.push_back(getRelativePolarPos(agent->pos, hostileAgent->pos, agent->dir));
	}

	for (NeuralWarfareEngine::Agent* friendlyAgent : friendlyAgents)
	{
		observation->friendlyAgents.push_back(getRelativePolarPos(agent->pos, friendlyAgent->pos, agent->dir));
	}

	return observation;
}

NeuralWarfareEnv::MyObservation::MyObservation()
{
}

NeuralWarfareEnv::MyObservation::~MyObservation()
{
	friendlyAgents.clear();
	hostileAgents.clear();
}

std::vector<double> NeuralWarfareEnv::MyObservation::GetForNN()
{
	std::vector<double> out;

	out.push_back(health);
	for (size_t i = 0; i < friendlyAgents.size(); i++)
	{
		out.push_back(friendlyAgents[i].first);
		out.push_back(friendlyAgents[i].second);
	}
	for (size_t i = 0; i < hostileAgents.size(); i++)
	{
		out.push_back(hostileAgents[i].first);
		out.push_back(hostileAgents[i].second);
	}
	return out;
}

double NeuralWarfareEnv::MyObservation::GetForTest()
{
	return hostileAgents.empty() ? 0 : hostileAgents.front().second;
}
