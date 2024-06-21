#include "NeuralWarfareEnv.h"
#include "angleTools.h"
#include "KDTree.h"

size_t NeuralWarfareEnv::MyObservation::hostileAgentCount = 2;
size_t NeuralWarfareEnv::MyObservation::friendlyAgentCount = 2;

NeuralWarfareEnv::NeuralWarfareEnv(NeuralWarfareEngine& engine, size_t teamNum) : engine(engine)
{
	ConnectToTeam(teamNum);
}

NeuralWarfareEnv::~NeuralWarfareEnv()
{

}

void NeuralWarfareEnv::TakeAction(std::list<Action*>& actions)
{
	for (Action* action : actions)
	{
		action->ExecuteAction(agents[action->ID]);
	}
}

std::list<Environment::StepResult>* NeuralWarfareEnv::GetResult()
{
	std::list<StepResult>* srts = new std::list<StepResult>();
	for (size_t i = 0; i < agents.size(); i++)
	{
		srts->emplace_back(getObservation(agents[i]), agents[i]->reward, agents[i]->health <= 0, engine.wasReset, i);
	}
	return srts;
}

std::list<Environment::StepResult>* NeuralWarfareEnv::Reset()
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
	MyObservation* observation = new MyObservation(engine,agent);


	return observation;
}

NeuralWarfareEnv::MyObservation::MyObservation(NeuralWarfareEngine& engine, NeuralWarfareEngine::Agent* agent)
{
	if (agent)
	{
		// use KD tree to find hostileAgents
		std::vector<NeuralWarfareEngine::Agent*> hostileAgentsVector =
			engine.kdTree.FindNearestNeighbors(agent->pos, hostileAgentCount,
				[agent](const NeuralWarfareEngine::Agent* a) {
					return a->teamId != agent->teamId && a->health > 0;
				}
		);
		// use KD tree to find friendlyAgents
		std::vector<NeuralWarfareEngine::Agent*> friendlyAgentsVector =
			engine.kdTree.FindNearestNeighbors(agent->pos, friendlyAgentCount,
				[agent](const NeuralWarfareEngine::Agent* a) {
					return a->teamId == agent->teamId && a != agent && a->health > 0;
				}
		);

		health = agent->health;

		for (NeuralWarfareEngine::Agent* hostileAgent : hostileAgentsVector)
		{
			hostileAgents.emplace_back(getRelativePolarPos(agent->pos, hostileAgent->pos, agent->dir));
		}

		for (NeuralWarfareEngine::Agent* friendlyAgent : friendlyAgentsVector)
		{
			friendlyAgents.emplace_back(getRelativePolarPos(agent->pos, friendlyAgent->pos, agent->dir));
		}
	}
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

size_t NeuralWarfareEnv::MyObservation::NNInputSize()
{
	return (hostileAgentCount + friendlyAgentCount) * 2 + 1;
}

double NeuralWarfareEnv::MyObservation::GetForTest()
{
	return hostileAgents.empty() ? 0 : hostileAgents.front().second;
}

void NeuralWarfareEnv::MyAction::ExecuteAction(void* ptr)
{
	NeuralWarfareEngine::Agent* agent = static_cast<NeuralWarfareEngine::Agent*>(ptr);
	agent->dir += action;
}

NeuralWarfareEnv::MyAction::MyAction(StepResult& sr) : Action(sr)
{
}

NeuralWarfareEnv::MyAction::MyAction(size_t ID) : Action(ID)
{
}

void NeuralWarfareEnv::MyAction::GetFromNN(std::vector<double> outputs)
{
	action = outputs.front() * std::numbers::pi * 2;
}

size_t NeuralWarfareEnv::MyAction::NNOutputSize()
{
	return 1;
}

void NeuralWarfareEnv::MyAction::GetFromTest(double value)
{
	//size_t action = value == 0 ? 0 : value < 0 ? 1 : 2;
	action = value;
}
