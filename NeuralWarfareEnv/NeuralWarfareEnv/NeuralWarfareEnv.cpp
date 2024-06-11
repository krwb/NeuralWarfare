#include "NeuralWarfareEnv.h"
#include "angleTools.h"
#include "KDTree.h"

std::vector<NeuralWarfareEngine::Agent*> getNearestNeighbors(NeuralWarfareEngine* eng, Vec2 query, size_t count, KDTree<NeuralWarfareEngine::Agent>::Condition condition = [](const NeuralWarfareEngine::Agent* a) { return true; })
{
	KDTree<NeuralWarfareEngine::Agent>::MaxHeap maxHeap;
	for (NeuralWarfareEngine::Agent& agent : eng->agents)
	{
		float distance = (query - agent.pos).Length();
		if (maxHeap.size() < count)
		{
			maxHeap.emplace(distance, &agent);
		}
		else if (distance < maxHeap.top().first && condition(&agent))
		{
			maxHeap.pop();
			maxHeap.emplace(distance, &agent);
		}
	}
	std::vector<NeuralWarfareEngine::Agent*> neighbors;
	while (!maxHeap.empty()) {
		neighbors.push_back(maxHeap.top().second);
		maxHeap.pop();
	}
	return neighbors;
}

std::vector<NeuralWarfareEngine::Agent*> getInRange(NeuralWarfareEngine* eng, Vec2 query, float range, KDTree<NeuralWarfareEngine::Agent>::Condition condition = [](const NeuralWarfareEngine::Agent* a) { return true; })
{
	std::vector<NeuralWarfareEngine::Agent*> agents;
	for (NeuralWarfareEngine::Agent& agent : eng->agents)
	{
		if ((agent.pos - query).Length() < range && condition(&agent))
		{
			agents.push_back(&agent);
		}
	}
	return agents;
}


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
	std::vector<StepResult*> sr;
	for (size_t i = 0; i < agents.size(); i++)
	{
		sr.push_back(new StepResult(getObservation(agents[i]), 0, false, false,i));
	}
	return sr;
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
	if (!hostileAgents.empty())
	{
		out.push_back(hostileAgents.front().second);
	}
	return out;
}
