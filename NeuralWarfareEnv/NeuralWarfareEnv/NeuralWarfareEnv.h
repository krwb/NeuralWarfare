#pragma once
#include "Environment.h"
#include "NeuralWarfareEngine.h"

/// <summary>
/// Environment that serves as a connection between the game engine and the AI
/// </summary>
class NeuralWarfareEnv : public Environment
{
public:
	/// <summary>
	/// NeuralWarfareEnv constructor
	/// </summary>
	/// <param name="engine"> Reference the game engine</param>
	/// <param name="teamID"> The team ID of the agents connected the environment</param>
	NeuralWarfareEnv(NeuralWarfareEngine& engine, size_t teamID);

	/// <summary>
	/// NeuralWarfareEnv destructor
	/// </summary>
	~NeuralWarfareEnv() override;

	/// <summary>
	/// Function to preform an action in the Environment
	/// </summary>
	/// <param name="actions"></param>
	void TakeAction(std::list<Action>& actions) override;

	/// <summary>
	/// Function to get the result of the last action preformed
	/// </summary>
	/// <returns>array of StepResult from the last taken action</returns>
	std::list<StepResult>* GetResult() override;

	/// <summary>
	/// Function to reset the environment
	/// </summary>
	/// <returns>array of StepResult from the reset state of the Environment</returns>
	std::list<StepResult>* Reset() override;


	/// <summary>
	/// Stores information for an AI to use to decide its next action.
	/// </summary>
	static class MyObservation : public Observation
	{
	public:
		MyObservation(NeuralWarfareEngine& eng, NeuralWarfareEngine::Agent* agent);
		~MyObservation() override;
		/// <summary>
		/// Converts the information for use by a neural network.
		/// </summary>
		/// <returns>The inputs for a neural network</returns>
		std::vector<double> GetForNN() override;

		virtual size_t NNInputSize() override;

		double GetForTest() override;

		float health; // Health of the agent
		std::vector<std::pair<float, double>> hostileAgents; // list of hostile Agents
		std::vector<std::pair<float, double>> friendlyAgents; // List of friendly Agents

		static size_t hostileAgentCount;
		static size_t friendlyAgentCount;
	private:

	};
private:


	size_t teamId; // The team ID of the agents connected the environment
	NeuralWarfareEngine& engine; // Reference the game engine
	std::vector<NeuralWarfareEngine::Agent*> agents; // array of pointers that this environment is training

	/// <summary>
	/// Gets the observation for a specific agent
	/// </summary>
	/// <param name="agent"></param>
	/// <returns>observation for a specific agent</returns>
	Observation* getObservation(NeuralWarfareEngine::Agent* agent);

	/// <summary>
	/// function to connect the Environment to a team in the engine
	/// </summary>
	/// <param name="teamNum"> the team to connect to</param>
	void ConnectToTeam(size_t teamId);
};
