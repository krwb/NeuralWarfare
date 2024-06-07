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
		Agent(size_t teamId, Vec2 pos, float health, double dir);
		~Agent();
		size_t teamId; // team ID
		Vec2 pos; // position
		double dir; // direction
		float health = 100;

		/// <summary>
		/// Makes the agent take an action
		/// </summary>
		/// <param name="action"></param>
		void TakeAction(size_t action);
		void UpdatePos(float delta);
		void Reset();
	private:
		Vec2 spawnPos;
		float baseHealth;
	};

	std::mt19937& gen; // random number generator ref
	Vec2 simSize;

	std::list<Agent> agents; // list of all agents in the simulation
	KDTree<Agent>* kdTree = nullptr;

	/// <summary>
	/// Creates a new team of agents
	/// </summary>
	/// <param name="numAgents"></param>
	/// <returns>The teamID of the created agents</returns>
	size_t AddTeam(size_t numAgents, float health, Vec2 pos);

	void RemoveTeam(size_t teamID);


	/// <summary>
	/// Primary update function for the engine
	/// </summary>
	/// <param name="delta">the duration of the update in seconds</param>
	void Update(float delta);

	/// <summary>
	/// Resets the engine
	/// </summary>
	/// <param name="delta"></param>
	void Reset();

	void Draw(Rectangle drawRec);
private:



	void UpdateKDTree();
	void DoCollisions(KDTree<Agent>::KDNode* node);
	void doCollision(Agent* agentA, Agent* agentB);
};


