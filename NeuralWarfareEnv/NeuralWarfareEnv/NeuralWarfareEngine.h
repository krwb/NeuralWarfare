#pragma once
#include <list>
#include <vector>
#include "raylib.h"
#include "KDTree.h"
#include <random>

/// <summary>
/// Engine for the NeuralWarfare environment
/// </summary>
/// <remarks>
/// This is necessary to allow teams to train individually whilst still interacting
/// </remarks>
class NeuralWarfareEngine
{
public:
	NeuralWarfareEngine(std::mt19937& gen, Vec2 simSize);
	~NeuralWarfareEngine();

	/// <summary>
	/// Agents are the basic players in the environment
	/// </summary>
	class Agent
	{
	public:
		Agent(size_t teamId, Vec2 pos, double dir);
		~Agent();
		size_t teamId; // team ID
		Vec2 pos; // position
		double dir; // direction
		float health = 1;

		/// <summary>
		/// Makes the agent take an action
		/// </summary>
		/// <param name="action"></param>
		void TakeAction(size_t action);
		void UpdatePos(float delta);
	private:

	};

	std::mt19937& gen; // random number generator

	std::list<Agent> agents; // list of all agents in the simulation

	KDTree<Agent>* kdTree = nullptr;

	Vec2 simSize;

	/// <summary>
	/// Primary update function for the engine
	/// </summary>
	/// <param name="delta">the duration of the update in seconds</param>
	void Update(float delta);

	/// <summary>
	/// Creates a new batch of agents
	/// </summary>
	/// <param name="numAgents"></param>
	/// <returns>The team of the created agents</returns>
	size_t AddTeam(size_t numAgents, Vec2 pos = {});

	void Draw(Rectangle drawRec);
private:

	void UpdateKDTree();
	void DoCollisions(KDTree<Agent>::KDNode* node);
	void doCollision(Agent* agentA, Agent* agentB);
};


