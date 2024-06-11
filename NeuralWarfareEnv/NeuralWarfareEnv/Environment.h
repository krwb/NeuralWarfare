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

		/// <summary>
		/// StepResult Constructor 
		/// </summary>
		/// <param name="observation"> used by an AI to determine next action</param>
		/// <param name="reward"> awarded to an AI, used for training</param>
		/// <param name="terminated"> Whether the AI was terminated before the episode has ended</param>
		/// <param name="truncated"> Whether the episode has ended</param>
		/// <param name="ID"> used to match an action to its step result in environments where there are multiple agents</param>
		StepResult(Observation* observation, float reward, bool terminated, bool truncated, size_t ID);

		/// <summary>
		/// StepResult destructor
		/// </summary>
		~StepResult();

		Environment::Observation* observation; // Observation from which the AI will determine its next action 
		float reward; // The reward granted to the AI for its action in the last step.
		bool terminated; // Whether the AI was terminated before the episode has ended.
		bool truncated; // Whether the episode has ended.

		size_t ID; // used to match an action to its step result in environments where there are multiple agents
	};

	/// <summary>
	/// Used to pair an agent with its action to prevent accidental crossover
	/// </summary>
	class Action
	{
	public:
		/// <summary>
		/// Action constructor
		/// </summary>
		/// <param name="sr"> used to obtain the ID</param>
		/// <param name="action"></param>
		Action(StepResult* sr, size_t action);

		/// <summary>
		/// Action constructor
		/// </summary>
		/// <param name="ID"> used to match the action to an agent</param>
		/// <param name="action"></param>
		Action(size_t ID, size_t action);

		/// <summary>
		/// Action destructor
		/// </summary>
		virtual ~Action();
		
		size_t ID; // used to match an action to its step result in environments where there are multiple 
		size_t action;
	};

	/// <summary>
	/// Function to preform an action in the Environment
	/// </summary>
	/// <param name="actions"></param>
	virtual void TakeAction(const std::vector<Action>& actions) = 0;
	
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

