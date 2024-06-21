#pragma once
#include <list>
#include <vector>
#include <random>
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
	/// <summary>
	/// Engine constructor
	/// </summary>
	/// <param name="gen"> reference to the random number generator</param>
	/// <param name="simSize"> the size of the simulation, measured from center </param>
	NeuralWarfareEngine(std::mt19937& gen, Vec2 simSize);

	/// <summary>
	/// Engine destructor
	/// </summary>
	~NeuralWarfareEngine();

	/// <summary>
	/// Agents are the basic players in the environment
	/// </summary>
	static class Agent
	{
	public:
		/// <summary>
		/// Agent constructor
		/// </summary>
		/// <param name="teamId"></param>
		/// <param name="pos"> spawn/start position</param>
		/// <param name="health"> base/start health</param>
		/// <param name="dir"> start direction in radians</param>
		Agent(size_t teamId, Vec2 pos, float health, double dir);

		/// <summary>
		/// Agent destructor
		/// </summary>
		~Agent();

		size_t teamId; // team ID
		Vec2 pos; // position
		double dir; // direction in radians
		float health; // agent health
		
		float reward = 0;

		/// <summary>
		/// update the position of the agent
		/// </summary>
		/// <param name="delta">the duration of the update</param>
		void UpdatePos(float delta);

		/// <summary>
		/// Resets the agent to spawnPos and baseHealth
		/// </summary>
		void Reset();

	private:
		Vec2 spawnPos; // Agent spawn/starting position
		float baseHealth; // agent spawn/starting health
	};

	std::mt19937& gen; // random number generator ref
	Vec2 simSize; // the size of the simulation, measured from center
	bool wasReset = false;

	std::list<Agent> agents; // list of all agents in the simulation
	KDTree<Agent> kdTree; // KD tree used for collision optimization and by environment observations

	/// <summary>
	/// Creates a new team of agents
	/// </summary>
	/// <param name="numAgents"></param>
	/// <returns>The teamID of the created agents</returns>
	size_t AddTeam(size_t numAgents, float health, Vec2 pos);

	/// <summary>
	/// Removes all agents matching the given teamID
	/// </summary>
	void RemoveTeam(size_t teamID);

	/// <summary>
	/// Primary update function for the engine
	/// </summary>
	/// <param name="delta">the duration of the update</param>
	void Update(float delta);

	/// <summary>
	/// Resets the engine
	/// </summary>
	void Reset();

	/// <summary>
	/// Primary draw function for the visualization
	/// </summary>
	/// <param name="drawRec"> used to scale and position the visualization on the window</param>
	void Draw(Rectangle drawRec);


	static Color GenerateTeamColor(size_t teamID);
private:

	/// <summary>
	/// Updates the KD tree by rebuilding it
	/// </summary>
	void UpdateKDTree();

	/// <summary>
	/// Collision detection function built to make use of the KD tree
	/// </summary>
	/// <param name="node"></param>
	void DoCollisions(KDTree<Agent>::KDNode* node);

	/// <summary>
	/// Collision function, used to define collision behavior
	/// </summary>
	/// <param name="agentA"></param>
	/// <param name="agentB"></param>
	void DoCollision(Agent* agentA, Agent* agentB);
};


