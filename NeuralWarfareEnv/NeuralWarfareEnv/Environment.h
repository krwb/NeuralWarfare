#pragma once
#include <vector>


/// <summary>
/// Mostly abstract class that defines methods a learning algorithm expects the Environment to have.
/// </summary>
class Environment
{
public:
	Environment();
	virtual ~Environment();
	/// <summary>
	/// Stores information for an AI to use to decide its next action.
	/// </summary>
	class Observation
	{
	public:
		Observation();
		virtual ~Observation();
		/// <summary>
		/// Converts the information for use by a neural network.
		/// </summary>
		/// <returns>The inputs for a neural network</returns>
		virtual std::vector<double> GetForNN() = 0;
	};

	/// <summary>
	/// Stores the information resulting from a step.
	/// </summary>
	class StepResult
	{
	public:
		StepResult(Observation* observation, float reward, bool terminated, bool truncated);
		~StepResult();

		Environment::Observation* observation; // Observation from which the AI will determine its next action 
		float reward; // The reward granted to the AI for its action in the last step.
		bool terminated; // Whether the AI was terminated before the episode has ended.
		bool truncated; // Whether the episode has ended.
	};

	/// <summary>
	/// Function to preform an action in the Environment
	/// </summary>
	/// <param name="actions"></param>
	virtual void TakeAction(const std::vector<size_t>& actions) = 0;
	
	/// <summary>
	/// Function to get the result of the last action preformed
	/// </summary>
	/// <returns>array of StepResult from the last taken action</returns>
	virtual std::vector<StepResult*> GetResult() = 0;

	/// <summary>
	/// Function to reset the environment
	/// </summary>
	/// <returns>array of StepResult from the reset state of the Environment</returns>
	virtual std::vector<StepResult*> Reset() = 0;

	private:

};

