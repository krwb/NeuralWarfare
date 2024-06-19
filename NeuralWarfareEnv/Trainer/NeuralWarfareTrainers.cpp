#include "NeuralWarfareTrainers.h"
#include <algorithm>
#include "tinyxml2.h"
#include "SimpleMutate.h"
#include <iostream>
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

GeneticAlgorithmNNTrainer::GeneticAlgorithmNNTrainer(Environment* env, std::mt19937& gen, MyHyperparameters hyperparameters, NeuralNetwork* masterNetwork) :  Trainer(env), gen(gen), hyperparameters(hyperparameters), masterNetwork(masterNetwork)
{
	agents.push_back(new Agent(masterNetwork));
}

GeneticAlgorithmNNTrainer::~GeneticAlgorithmNNTrainer()
{
	while (!agents.empty())
	{
		delete agents.back();
		agents.pop_back();
	}
}

void GeneticAlgorithmNNTrainer::Update()
{
	if (LastStepResults)
	{
		while (agents.size() < LastStepResults->size())
		{
			agents.push_back(new Agent { NeuralNetwork::Copy(masterNetwork) });
		}
		nextActions = new std::list<Environment::Action>();
		bool allTruncated = true;
		for (Environment::StepResult& sr : *LastStepResults)
		{
			agents[sr.ID]->fitness += sr.reward;
			if (!sr.truncated) { allTruncated = false; }
		}
		if (allTruncated)
		{
			Evolve();
		}
		for (Environment::StepResult& sr : *LastStepResults)
		{
			size_t action = 0;
			if (!(sr.terminated))
			{
				std::vector<double> nnOutputs = agents[sr.ID]->network->Evaluate(sr.observation->GetForNN());
				double maxValue = 0;
				for (size_t i = 0; i < nnOutputs.size(); i++)
				{
					if (maxValue < nnOutputs[i])
					{
						maxValue = nnOutputs[i];
						action = i;
					}
				}
			}
			nextActions->emplace_back(sr, action);
		}
	}
}

void GeneticAlgorithmNNTrainer::SetNewLayerFunction()
{
	std::unordered_map<std::string, ActivationFunction*> functionMap;
	for (ActivationFunction* function : masterNetwork->functions)
	{
		functionMap[function->name] = function;
	}
	newLayerFunction = functionMap.at(hyperparameters.newLayerFunction);
}

void GeneticAlgorithmNNTrainer::Evolve()
{
	if (!newLayerFunction)
	{
		SetNewLayerFunction();
	}
	std::vector<Agent*>::iterator topAgentsEnd = agents.begin() + hyperparameters.topAgentCount;
	std::partial_sort(agents.begin(), topAgentsEnd, agents.end(), [](Agent* a, Agent* b) { return a->fitness > b->fitness; });
	masterNetwork = agents.front()->network;
	{
		std::vector<Agent*>::iterator otherAgentsIter = topAgentsEnd;
		std::vector<Agent*>::iterator topAgentsIter = agents.begin();
		while (otherAgentsIter != agents.end())
		{
			if (topAgentsIter == topAgentsEnd) { topAgentsIter = agents.begin(); }

			if ((*otherAgentsIter)->network == (*topAgentsIter)->network)
			{
				std::cout << "test";
			}
			(*otherAgentsIter)->SetNetwork(NeuralNetwork::Copy((*topAgentsIter)->network));

			for (size_t i = 0; i < hyperparameters.mutationCouunt; i++)
			{
				SimpleMutate(
					(*otherAgentsIter)->network,
					gen,
					hyperparameters.biasMutationRate,
					hyperparameters.biasMutationMagnitude,
					hyperparameters.weightMutationRate,
					hyperparameters.weightMutationMagnitude,
					hyperparameters.synapseMutationRate,
					hyperparameters.newSynapseMagnitude,
					hyperparameters.nodeMutationRate,
					hyperparameters.layerMutationRate,
					hyperparameters.newLayerSizeAverage,
					hyperparameters.newLayerSizeRange,
					newLayerFunction
					);
			}
			//(*otherAgentsIter)->network->Clean();
			otherAgentsIter++;
			topAgentsIter++;
		}
	}
}

void GeneticAlgorithmNNTrainer::MyHyperparameters::Load(std::string fileName)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS) {
		return;
	}

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root) {
		return;
	}

	// Read attributes from root element
	root->QueryUnsigned64Attribute("topAgentCount", &topAgentCount);
	root->QueryUnsigned64Attribute("mutationCouunt", &mutationCouunt);
	root->QueryDoubleAttribute("biasMutationRate", &biasMutationRate);
	root->QueryDoubleAttribute("biasMutationMagnitude", &biasMutationMagnitude);
	root->QueryDoubleAttribute("weightMutationRate", &weightMutationRate);
	root->QueryDoubleAttribute("weightMutationMagnitude", &weightMutationMagnitude);
	root->QueryDoubleAttribute("synapseMutationRate", &synapseMutationRate);
	root->QueryDoubleAttribute("newSynapseMagnitude", &newSynapseMagnitude);
	root->QueryDoubleAttribute("nodeMutationRate", &nodeMutationRate);
	root->QueryDoubleAttribute("layerMutationRate", &layerMutationRate);
	root->QueryIntAttribute("newLayerSizeAverage", &newLayerSizeAverage);
	root->QueryIntAttribute("newLayerSizeRange", &newLayerSizeRange);
	newLayerFunction = root->Attribute("newLayerFunction");
}

void GeneticAlgorithmNNTrainer::MyHyperparameters::Save(std::string fileName)
{
	tinyxml2::XMLDocument doc;

	// Declaration
	tinyxml2::XMLDeclaration* decl = doc.NewDeclaration();
	doc.LinkEndChild(decl);

	// Root element
	tinyxml2::XMLElement* root = doc.NewElement("Hyperparameters");
	doc.LinkEndChild(root);

	// Add attributes to root element
	root->SetAttribute("topAgentCount", topAgentCount);
	root->SetAttribute("mutationCouunt", mutationCouunt);
	root->SetAttribute("biasMutationRate", biasMutationRate);
	root->SetAttribute("biasMutationMagnitude", biasMutationMagnitude);
	root->SetAttribute("weightMutationRate", weightMutationRate);
	root->SetAttribute("weightMutationMagnitude", weightMutationMagnitude);
	root->SetAttribute("synapseMutationRate", synapseMutationRate);
	root->SetAttribute("newSynapseMagnitude", newSynapseMagnitude);
	root->SetAttribute("nodeMutationRate", nodeMutationRate);
	root->SetAttribute("layerMutationRate", layerMutationRate);
	root->SetAttribute("newLayerSizeAverage", newLayerSizeAverage);
	root->SetAttribute("newLayerSizeRange", newLayerSizeRange);
	root->SetAttribute("newLayerFunction", newLayerFunction.c_str());

	// Save to file
	doc.SaveFile(fileName.c_str());
}

GeneticAlgorithmNNTrainer::Agent::Agent(NeuralNetwork* network) : network(network)
{

}
GeneticAlgorithmNNTrainer::Agent::~Agent()
{

}

void GeneticAlgorithmNNTrainer::Agent::SetNetwork(NeuralNetwork* newNetwork)
{
	network->Delete();
	network = newNetwork;
}
