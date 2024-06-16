#pragma once
#include "Environment.h"
#include <string>
#include "NeuralWarfareEnv.h"

class Trainer
{
public:
	class Hyperparameters
	{
	public:
		Hyperparameters() {}
		virtual ~Hyperparameters() {}
		virtual void Load(std::string fileName) = 0;
		virtual void Save(std::string fileName) = 0;

	private:

	};
	Trainer(Environment* env) : env(env)
	{
		NeuralWarfareEnv* e = reinterpret_cast<NeuralWarfareEnv*>(env);
		int a = 3;
	}
	virtual ~Trainer() {};
	void Look() 
	{ 
		if (LastStepResults) { delete LastStepResults; }
		LastStepResults = env->GetResult(); 
	}
	virtual void Think() = 0;
	void Do()
	{
		if (nextActions)
		{
			env->TakeAction(*nextActions);
			delete nextActions;
			nextActions = nullptr;
		}
	}
protected:
	Environment* env;
	std::list<Environment::StepResult>* LastStepResults = nullptr;
	std::list<Environment::Action>* nextActions = nullptr;
};
