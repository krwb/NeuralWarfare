#include "NeuralWarfareTrainers.h"
TestTrainer::TestTrainer(Environment* env) : Trainer(env)
{

}

TestTrainer::~TestTrainer()
{

}

void TestTrainer::Update()
{
	if (LastStepResults)
	{
		nextActions = new std::list<Environment::Action>();
		for (Environment::StepResult& sr : *LastStepResults)
		{
			double nearestHostileDirection = sr.observation->GetForTest();
			size_t action = nearestHostileDirection == 0 ? 0 : nearestHostileDirection < 0 ? 1 : 2;
			nextActions->push_back({ sr, action });
		}
	}
}

StaticNNTrainer::StaticNNTrainer(Environment* env, NeuralNetwork* network) : Trainer(env), network(network)
{

}

StaticNNTrainer::~StaticNNTrainer()
{

}

void StaticNNTrainer::Update()
{
	if (LastStepResults)
	{
		nextActions = new std::list<Environment::Action>();
		for (Environment::StepResult& sr : *LastStepResults)
		{
			std::vector<double> outputs = network->Evaluate(sr.observation->GetForNN());
			size_t action = std::distance(outputs.begin(), std::max_element(outputs.begin(), outputs.end()));
			nextActions->push_back({ sr, action });
		}
	}
}

