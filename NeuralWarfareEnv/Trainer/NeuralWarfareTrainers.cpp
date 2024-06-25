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
			(*otherAgentsIter)->network->Clean();
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
	if(size_t e = root->QueryUnsigned64Attribute("topAgentCount", &topAgentCount) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'topAgentCount' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryUnsigned64Attribute("mutationCouunt", &mutationCouunt) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'mutationCouunt' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("biasMutationRate", &biasMutationRate) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'biasMutationRate' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("biasMutationMagnitude", &biasMutationMagnitude) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'biasMutationMagnitude' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("weightMutationRate", &weightMutationRate) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'weightMutationRate' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("weightMutationMagnitude", &weightMutationMagnitude) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'weightMutationMagnitude' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("synapseMutationRate", &synapseMutationRate) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'synapseMutationRate' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("newSynapseMagnitude", &newSynapseMagnitude) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'newSynapseMagnitude' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("nodeMutationRate", &nodeMutationRate) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'nodeMutationRate' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryDoubleAttribute("layerMutationRate", &layerMutationRate) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'layerMutationRate' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryIntAttribute("newLayerSizeAverage", &newLayerSizeAverage) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'newLayerSizeAverage' : TinyXMLError[" << e << "]\n";
	if(size_t e = root->QueryIntAttribute("newLayerSizeRange", &newLayerSizeRange) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load hyperperameter 'newLayerSizeRange' : TinyXMLError[" << e << "]\n";
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

NEATTrainer::NEATTrainer(Environment* env, std::mt19937& gen, ActivationFunction* newLayerFunction, size_t populationSize)
	: gen(gen), newLayerFunction(newLayerFunction), environment(env), populationSize(populationSize), compatibilityThreshold(3.0)
{
	// Initialize agents and their networks
	InitializeAgents();
}

NEATTrainer::~NEATTrainer()
{
	for (auto agent : agents) {
		delete agent;
	}
	agents.clear();
}


void NEATTrainer::InitializeAgents()
{
	// Initialize agents with initial networks
	for (size_t i = 0; i < populationSize; i++)
	{
		NeuralNetwork* initialNetwork = NeuralNetwork::Copy(masterNetwork);
		agents.push_back(new Agent(initialNetwork, new NEATNetworkInterface(initialNetwork)));
	}
}
void NEATTrainer::Update()
{
}

void NEATTrainer::Evolve()
{
}


NEATTrainer::Agent::Agent()
{
}

void NEATTrainer::Agent::SetNetwork()
{
	std::vector<ActivationFunction*>& functions = network->functions;
	network->Delete();
	network = neatInterface->ConstructNetworkFromGenes(functions);
}
