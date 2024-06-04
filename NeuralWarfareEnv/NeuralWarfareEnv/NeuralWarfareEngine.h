#pragma once
#include <list>
#include "Vec2.h"
#include <vector>
#include "raylib.h"
#include "KDTree.h"

/// <summary>
/// Engine for the NeuralWarfare environment
/// </summary>
/// <remarks>
/// This is necessary to allow teams to train individually whilst still interacting
/// </remarks>
class NeuralWarfareEngine
{
public:
	NeuralWarfareEngine(Vec2 simSize);
	~NeuralWarfareEngine();

	/// <summary>
	/// Agents are the basic players in the environment
	/// </summary>
	class Agent
	{
	public:
		Agent(size_t teamId);
		~Agent();
		size_t teamId; // team ID
		Vec2 pos; // position
		Vec2 dir; // direction
		float health = 1;

		/// <summary>
		/// Makes the agent take an action
		/// </summary>
		/// <param name="action"></param>
		void TakeAction(size_t action);
		void UpdatePos(float delta);
	private:

	};

	std::list<Agent> agents; // list of all agents in the simulation

	KDTree<Agent>* kdTree = nullptr;

	Vec2 simSize;

	/// <summary>
	/// Primary update function for the engine
	/// </summary>
	/// <param name="delta">the duration of the update in seconds</param>
	void Update(float delta);

	void UpdateKDTree();

	/// <summary>
	/// Creates a new batch of agents
	/// </summary>
	/// <param name="numAgents"></param>
	/// <returns>The team of the created agents</returns>
	size_t AddTeam(size_t numAgents);

	void Draw(Rectangle drawRec);
private:
};
