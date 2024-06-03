#pragma once
#include "Environment.h"
#include "NeuralWarfareEngine.h"


/// <summary>
/// Environment that serves as a connection between the game engine and the AI
/// </summary>
class NeuralWarfareEnv : public Environment
{
public:

	NeuralWarfareEnv(NeuralWarfareEngine& engine, size_t teamNum);
	~NeuralWarfareEnv();

	/// <summary>
	/// Function to preform an action in the Environment
	/// </summary>
	/// <param name="actions"></param>
	void TakeAction(const std::vector<size_t>& actions) override;

	/// <summary>
	/// Function to get the result of the last action preformed
	/// </summary>
	/// <returns>array of StepResult from the last taken action</returns>
	std::vector<StepResult> GetResult() override;

	/// <summary>
	/// Function to reset the environment
	/// </summary>
	/// <returns>array of StepResult from the reset state of the Environment</returns>
	std::vector<StepResult> Reset() override;

private:
	/// <summary>
	/// Stores information for an AI to use to decide its next action.
	/// </summary>
	class MyObservation : public Observation
	{
	public:
		/// <summary>
		/// Converts the information for use by a neural network.
		/// </summary>
		/// <returns>The inputs for a neural network</returns>
		std::vector<double> GetForNN() override;
	private:
		float health;
		std::vector<NeuralWarfareEngine::Agent*> hostileAgents;
		std::vector<NeuralWarfareEngine::Agent*> friendlyAgents;
	};


	NeuralWarfareEngine& engine; // Reference to an instance of the game engine

	size_t teamId; // The team connected the environment

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
