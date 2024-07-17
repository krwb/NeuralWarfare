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
		nextActions = new std::list<Environment::Action*>();
		for (Environment::StepResult& sr : *LastStepResults)
		{
			Environment::Action* action = new NeuralWarfareEnv::MyAction(sr);
			action->GetFromTest(sr.observation->GetForTest());
			nextActions->push_back(action);
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
		nextActions = new std::list<Environment::Action*>();
		for (Environment::StepResult& sr : *LastStepResults)
		{
			Environment::Action* action = new NeuralWarfareEnv::MyAction(sr);
			if (!(sr.terminated))
			{
				action->GetFromNN(network->Evaluate(sr.observation->GetForNN()));
			}
			nextActions->push_back(action);
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
		nextActions = new std::list<Environment::Action*>();
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
			Environment::Action* action = new NeuralWarfareEnv::MyAction(sr);
			if (!(sr.terminated))
			{
				action->GetFromNN(agents[sr.ID]->network->Evaluate(sr.observation->GetForNN()));
			}
			nextActions->push_back(action);
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

			for (size_t i = 0; i < hyperparameters.mutationCount; i++)
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
			(*otherAgentsIter)->network->Clean();
			otherAgentsIter++;
			topAgentsIter++;
		}
	}
}

void GeneticAlgorithmNNTrainer::MyHyperparameters::Load(std::string fileName)
{
	tinyxml2::XMLError e;
	tinyxml2::XMLDocument doc;

	e = doc.LoadFile(fileName.c_str());
	if (e != tinyxml2::XML_SUCCESS) {
		std::cerr << "Error loading XML file: " << fileName << " TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
		return;
	}

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root) {
		std::cerr << "Error: No root element found in XML file." << std::endl;
		return;
	}

	if ((e = root->QueryUnsigned64Attribute("topAgentCount", &topAgentCount)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'topAgentCount' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryUnsigned64Attribute("mutationCount", &mutationCount)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'mutationCount' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("biasMutationRate", &biasMutationRate)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'biasMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("biasMutationMagnitude", &biasMutationMagnitude)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'biasMutationMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("weightMutationRate", &weightMutationRate)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'weightMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("weightMutationMagnitude", &weightMutationMagnitude)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'weightMutationMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("synapseMutationRate", &synapseMutationRate)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'synapseMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("newSynapseMagnitude", &newSynapseMagnitude)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'newSynapseMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("nodeMutationRate", &nodeMutationRate)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'nodeMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryDoubleAttribute("layerMutationRate", &layerMutationRate)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'layerMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryIntAttribute("newLayerSizeAverage", &newLayerSizeAverage)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'newLayerSizeAverage' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
	if ((e = root->QueryIntAttribute("newLayerSizeRange", &newLayerSizeRange)) != tinyxml2::XML_SUCCESS)
		std::cerr << "Error: Failed to load hyperparameter 'newLayerSizeRange' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;

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
	root->SetAttribute("mutationCount", mutationCount);
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
