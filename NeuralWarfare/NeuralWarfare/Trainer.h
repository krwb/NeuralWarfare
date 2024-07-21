#pragma once
#include <string>
#include "Environment.h"

/// <summary>
/// Base class for training agents in an environment.
/// </summary>
class Trainer
{
public:
	/// <summary>
	/// Class representing hyperparameters for the trainer.
	/// </summary>
	static class Hyperparameters
	{
	public:
		/// <summary>
		/// Constructs default hyperparameters.
		/// </summary>
		Hyperparameters() {}

		/// <summary>
		/// Destructor for Hyperparameters.
		/// </summary>
		virtual ~Hyperparameters() {}

		/// <summary>
		/// Loads hyperparameters from a file.
		/// </summary>
		/// <param name="fileName">The file from which to load hyperparameters.</param>
		virtual void Load(std::string fileName) = 0;

		/// <summary>
		/// Saves hyperparameters to a file.
		/// </summary>
		/// <param name="fileName">The file to which to save hyperparameters.</param>
		virtual void Save(std::string fileName) = 0;

	private:

	};

	/// <summary>
	/// Constructs a trainer for a given environment.
	/// </summary>
	/// <param name="env">Pointer to the environment to be used for training.</param>
	Trainer(Environment* env) : env(env) {}

	/// <summary>
	/// Virtual destructor for Trainer.
	/// </summary>
	virtual ~Trainer() {}

	/// <summary>
	/// Observes the environment and updates the last step results.
	/// </summary>
	void ObserveEnvironment()
	{
		if (LastStepResults) { delete LastStepResults; }
		LastStepResults = env->GetResult();
	}

	/// <summary>
	/// Updates the trainer. This function is responsible for both determining actions and training the AI.
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// Executes the next set of actions in the environment.
	/// </summary>
	void ExecuteAction()
	{
		if (nextActions)
		{
			env->TakeAction(*nextActions);
			while (!nextActions->empty())
			{
				delete nextActions->front();
				nextActions->pop_front();
			}
			delete nextActions;
			nextActions = nullptr;
		}
	}

	Environment* env; // Pointer to the environment being used for training.
	bool training = false;
protected:
	std::list<Environment::StepResult>* LastStepResults = nullptr; // List of the last step results observed from the environment.
	std::list<Environment::Action*>* nextActions = nullptr; // List of the next actions to be executed in the environment.
};

static void UpdateTrainers(std::vector<Trainer*>& trainers)
{
	for (Trainer* trainer : trainers)
	{
		trainer->Update();
	}
}
