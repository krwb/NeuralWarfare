#pragma once
#include <list>
#include "Vec2.h"

/// <summary>
/// Engine for the NeuralWarfare environment
/// </summary>
/// <remarks>
/// This is necessary to allow teams to train individually whilst still interacting
/// </remarks>
class NeuralWarfareEngine
{
public:
	/// <summary>
	/// Agents are the basic players in the environment
	/// </summary>
	class Agent
	{
	public:
		Agent();
		~Agent();
		size_t team; // team ID
		Vec2 pos; // position
		Vec2 dir; // direction

		/// <summary>
		/// Makes the agent take an action
		/// </summary>
		/// <param name="action"></param>
		void TakeAction(size_t action);
		void UpdatePos();
	private:

	};

	std::list<Agent> agents; // list of all agents in the simulation

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
	size_t AddTeam(size_t numAgents);

private:

};
