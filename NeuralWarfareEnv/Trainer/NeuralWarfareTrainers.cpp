#include "NeuralWarfareTrainers.h"
#include "tinyxml2.h"
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

GeneticAlgorithmNNTrainer::GeneticAlgorithmNNTrainer(Environment* env, MyHyperparameters hyperparameters, NeuralNetwork* masterNetwork) : Trainer(env), hyperparameters(hyperparameters), masterNetwork(masterNetwork)
{
}

GeneticAlgorithmNNTrainer::~GeneticAlgorithmNNTrainer()
{
}

void GeneticAlgorithmNNTrainer::Update()
{
	if (LastStepResults)
	{
		while (networks.size() < LastStepResults->size())
		{
			networks.push_back(NeuralNetwork::Copy(masterNetwork));
		}
		if (!newLayerFunction)
		{
			SetNewLayerFunction();
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
