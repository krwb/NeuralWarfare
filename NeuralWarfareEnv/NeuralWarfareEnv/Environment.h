#pragma once
#include <vector>
#include <list>

/// <summary>
/// Mostly abstract class that defines methods a learning algorithm expects the Environment to have.
/// </summary>
class Environment
{
public:
	Environment() {};
	virtual ~Environment() {};
	/// <summary>
	/// Stores information for an AI to use to decide its next action.
	/// </summary>
	static class Observation
	{
	public:
		Observation() {};
		virtual ~Observation() {};
		/// <summary>
		/// Converts the information for use by a neural network.
		/// </summary>
		/// <returns>The inputs for a neural network</returns>
		virtual std::vector<double> GetForNN() = 0;

		virtual size_t NNInputSize() = 0;

		virtual double GetForTest() = 0;
	};


	/// <summary>
	/// Stores the information resulting from a step.
	/// </summary>
	static class StepResult
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
		StepResult(Observation* observation, float reward, bool terminated, bool truncated, size_t ID) :
			observation(observation),
			reward(reward),
			terminated(terminated),
			truncated(truncated),
			ID(ID) {}

		/// <summary>
		/// StepResult destructor
		/// </summary>
		~StepResult() { delete observation; }

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
		Action(StepResult& sr) :
			ID(sr.ID) {}

		/// <summary>
		/// Action constructor
		/// </summary>
		/// <param name="ID"> used to match the action to an agent</param>
		/// <param name="action"></param>
		Action(size_t ID) :
			ID(ID) {}

		/// <summary>
		/// Action destructor
		/// </summary>
		virtual ~Action() {};

		virtual size_t NNOutputSize() = 0;

		virtual void GetFromNN(std::vector<double> outputs) = 0;
		
		virtual void GetFromTest(double value) = 0;

		virtual void ExecuteAction(void* ptr) = 0;

		size_t ID; // used to match an action to its step result in environments where there are multiple 
	};

	/// <summary>
	/// Function to preform an action in the Environment
	/// </summary>
	/// <param name="actions"></param>
	virtual void TakeAction(std::list<Action*>& actions) = 0;
	
	/// <summary>
	/// Function to get the result of the last action preformed
	/// </summary>
	/// <returns>array of StepResult from the last taken action</returns>
	virtual std::list <StepResult>* GetResult() = 0;

	/// <summary>
	/// Function to reset the environment
	/// </summary>
	/// <returns>array of StepResult from the reset state of the Environment</returns>
	virtual std::list<StepResult>* Reset() = 0;

	private:

};

