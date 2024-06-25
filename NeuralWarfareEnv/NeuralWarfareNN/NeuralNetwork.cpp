#include "NeuralNetwork.h"
#include <queue>
#include <unordered_map>
#include <fstream>

NeuralNetwork::NeuralNetwork(std::vector<ActivationFunction*>& functions) : functions(functions)
{
	for (size_t i = 0; i < 2; i++)
	{
		new Layer(this);
	}
}

NeuralNetwork::~NeuralNetwork()
{
	while (!layers.empty())
	{
		delete layers.front();
		pop_front();
	}
}

void NeuralNetwork::AddInput(Node* newNode)
{
	inputNodes.push_back(newNode);
	newNode->SetLayer(layers.front());
}

void NeuralNetwork::AddOutput(Node* newNode)
{
	outputNodes.push_back(newNode);
	newNode->SetLayer(layers.back());
}

std::vector<double> NeuralNetwork::Evaluate(std::vector<double> inputValues)
{
	for (size_t i = 0; i < inputValues.size() && i < inputNodes.size(); i++)
	{
		inputNodes[i]->inputValue += inputValues[i];
	}

	Update();

	std::vector<double> outputValues(outputNodes.size());
	for (size_t i = 0; i < outputNodes.size(); i++)
	{
		outputValues[i] = outputNodes[i]->outputValue;
	}

	return outputValues;
}

void NeuralNetwork::Update()
{
	for (Layer* layer : layers)
	{
		for (Node* node : *layer)
		{
			node->Update();
		}
	}
}

void NeuralNetwork::MakeFullyConnected()
{
	std::list<Layer*>::iterator layerIter = layers.begin();;
	while (layerIter != std::prev(layers.end()))
	{
		for (Node* nodeA : **layerIter)
		{
			for (Node* nodeB : **std::next(layerIter))
			{
				new Synapse(nodeA, nodeB, 1);
			}
		}
		layerIter++;
	}
}

void CombineDuplicateSynapses(std::list<Synapse*>& synapses)
{
	std::list<Synapse*>::iterator synapseIterA = synapses.begin();
	while (synapseIterA != synapses.end())
	{
		std::list<Synapse*>::iterator synapseIterB = std::next(synapseIterA);
		while (synapseIterB != synapses.end())
		{
			if (((*synapseIterA)->in == (*synapseIterB)->in) && ((*synapseIterA)->out == (*synapseIterB)->out))
			{
				(*synapseIterA)->weight += (*synapseIterB)->weight;
				Synapse* extraSynapse = (*synapseIterB);
				synapseIterB++;
				delete extraSynapse;
				continue;
			}
			synapseIterB++;
		}
		synapseIterA++;
	}
}

void NeuralNetwork::CleanSynapses()
{
	iterator layerIter = layers.begin();
	while (layerIter != std::prev(layers.end()))
	{
		Layer::iterator nodeIter = (*layerIter)->begin();
		// Iterate through nodes
		while (nodeIter != (*layerIter)->end())
		{
			CombineDuplicateSynapses((*nodeIter)->outputs);
			nodeIter++;
		}
		layerIter++;
	}
}

void NeuralNetwork::CleanNodes()
{
	iterator layerIter = std::next(layers.begin());
	std::queue<Node*> deleteQueue;
	while (layerIter != std::prev(layers.end()))
	{
		Layer::iterator nodeIter = (*layerIter)->begin();
		// Iterate through nodes
		while (nodeIter != (*layerIter)->end())
		{
			if ((*nodeIter)->inputs.empty() && (*nodeIter)->outputs.empty())
			{
				deleteQueue.push(*nodeIter);
			}
			nodeIter++;
		}
		layerIter++;
	}
	while (!deleteQueue.empty())
	{
		deleteQueue.front()->Delete();
		deleteQueue.pop();
	}
}


void NeuralNetwork::Clean()
{
	CleanNodes();
	CleanSynapses();
}

std::vector<char> NeuralNetwork::GetBin(const NeuralNetwork& network)
{
	std::vector<char> data;

	// Saving the number of functions
	AppendToData(data, network.functions.size());

	std::unordered_map<const ActivationFunction*, size_t> funcMap;

	// Mapping functions to their indices and saving function names
	for (size_t i = 0; i < network.functions.size(); i++) {
		funcMap[(network.functions)[i]] = i;
		AppendToData(data, network.functions[i]->name);
	}

	// Saving the number of layers
	size_t layerCount = network.size();
	AppendToData(data, layerCount);

	std::unordered_map<const Node*, size_t> nodeMap;
	int nodeIndex = 0;

	// Mapping nodes to their indices and saving layer sizes
	for (Layer* layer : network) {
		AppendToData(data, layer->size());
		for (Node* node : *layer) {
			nodeMap[node] = nodeIndex++;
		}
	}

	// Saving node details and their synapses
	for (Layer* layer : network) {
		for (Node* node : *layer) {
			AppendToData(data, funcMap[node->function]);
			AppendToData(data, node->bias);
			AppendToData(data, node->outputs.size());

			for (Synapse* synapse : node->outputs) {
				AppendToData(data, nodeMap[synapse->out]);
				AppendToData(data, synapse->weight);
			}
		}
	}
	return data;
}

void NeuralNetwork::Save(const NeuralNetwork& network, std::string filename)
{
	std::ofstream file(filename, std::ios::binary);
	std::vector<char> data = GetBin(network);
	file.write(data.data(), data.size());
	file.close();
	return;
}

NeuralNetwork* NeuralNetwork::MakeFromBin(std::vector<char>& data, std::vector<ActivationFunction*>& avalableFunctions, size_t& offset)
{
	std::unordered_map<std::string, ActivationFunction*> functionMap;
	for (ActivationFunction* function : avalableFunctions)
	{
		functionMap[function->name] = function;
	}

	// Loading the number of functions
	size_t numFuncs;
	ExtractFromData(data, offset, numFuncs);
	std::vector<ActivationFunction*> loadedFunctions(numFuncs);

	// Mapping functions to their indices
	try
	{
		for (size_t i = 0; i < numFuncs; ++i) {
			std::string funcName;
			ExtractFromData(data, offset, funcName);
			(loadedFunctions)[i] = functionMap.at(funcName);
		}
	}
	catch (const std::out_of_range& e)
	{
		return nullptr;
	}
	// Loading the number of layers
	size_t numLayers;
	ExtractFromData(data, offset, numLayers);
	std::vector<size_t> layerSizes(numLayers);

	size_t nodeCount = 0;
	for (size_t i = 0; i < numLayers; ++i) {
		ExtractFromData(data, offset, layerSizes[i]);
		nodeCount += layerSizes[i];
	}

	std::vector<Node*> nodes;
	for (size_t i = 0; i < nodeCount; i++)
	{
		nodes.push_back(new Node(nullptr, nullptr));
	}

	// Loading nodes
	for (size_t i = 0; i < nodeCount; ++i) {
		size_t funcIndex;
		double bias;
		ExtractFromData(data, offset, funcIndex);
		ExtractFromData(data, offset, bias);
		nodes[i]->function = (loadedFunctions)[funcIndex];
		nodes[i]->bias = bias;
		size_t synapseCount;
		ExtractFromData(data, offset, synapseCount);
		for (size_t s = 0; s < synapseCount; ++s) {
			size_t outIndex;
			double weight;
			ExtractFromData(data, offset, outIndex);
			ExtractFromData(data, offset, weight);
			new Synapse(nodes[i], nodes[outIndex], weight);
		}
	}

	// Reconstructing the network
	NeuralNetwork* network = new NeuralNetwork(avalableFunctions);
	size_t n = 0;
	for (size_t i = 0; i < layerSizes.front(); i++)
	{
		network->AddInput(nodes[n++]);
	}

	for (size_t i = 1; i < numLayers - 1; ++i) {
		Layer* layer = new Layer(network, std::prev(network->end()));
		for (size_t j = 0; j < layerSizes[i]; ++j) {
			nodes[n++]->SetLayer(layer);
		}
	}

	for (size_t i = 0; i < layerSizes.back(); i++)
	{
		network->AddOutput(nodes[n++]);
	}

	return network;
}

NeuralNetwork* NeuralNetwork::Load(std::vector<ActivationFunction*>& functions, std::string filename)
{
	// Open the file in binary mode
	std::ifstream file(filename, std::ios::binary);

	// Check if the file opened successfully
	if (!file) {
		throw std::runtime_error("Unable to open file: test.bin");
	}

	// Move the file pointer to the end to get the file size
	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Resize the vector to hold the file contents
	std::vector<char> data(fileSize);

	// Read the file contents into the vector
	file.read(data.data(), fileSize);

	// Close the file
	file.close();
	size_t offset = 0;
	return MakeFromBin(data, functions, offset);
}

void NeuralNetwork::Delete()
{
	delete this;
}

NeuralNetwork* NeuralNetwork::Copy(NeuralNetwork* oldNetwork)
{
	NeuralNetwork* newNetwork = new NeuralNetwork(oldNetwork->functions);
	std::unordered_map<Node*, Node*> nodeMap;
	while (newNetwork->size() < oldNetwork->size())
	{
		new Layer(newNetwork);
	}

	{
		iterator oldLayerIter = oldNetwork->begin();
		iterator newLayerIter = newNetwork->begin();
		while (oldLayerIter != oldNetwork->end() && newLayerIter != newNetwork->end())
		{
			Layer::iterator oldNodeIter = (*oldLayerIter)->begin();
			while (oldNodeIter != (*oldLayerIter)->end())
			{
				Node* newNode = new Node(*newLayerIter, (*oldNodeIter)->function, (*oldNodeIter)->bias);
				nodeMap.insert({ (*oldNodeIter),newNode });
				oldNodeIter++;
			}
			oldLayerIter++;
			newLayerIter++;
		}
	}

	{
		iterator oldLayerIter = oldNetwork->begin();
		iterator newLayerIter = newNetwork->begin();
		while (oldLayerIter != oldNetwork->end() && newLayerIter != newNetwork->end())
		{
			Layer::iterator oldNodeIter = (*oldLayerIter)->begin();
			Layer::iterator newNodeIter = (*newLayerIter)->begin();
			while (oldNodeIter != (*oldLayerIter)->end() && newNodeIter != (*newLayerIter)->end())
			{
				for (Synapse* synapse : (*oldNodeIter)->outputs)
				{
					new Synapse(*newNodeIter, nodeMap[synapse->out], synapse->weight);
				}
				oldNodeIter++;
				newNodeIter++;
			}
			oldLayerIter++;
			newLayerIter++;
		}
	}
	newNetwork->inputNodes.resize(oldNetwork->inputNodes.size());

	for (size_t i = 0; i < oldNetwork->inputNodes.size(); i++)
	{
		newNetwork->inputNodes[i] = nodeMap[oldNetwork->inputNodes[i]];
	}

	newNetwork->outputNodes.resize(oldNetwork->outputNodes.size());

	for (size_t i = 0; i < oldNetwork->outputNodes.size(); i++)
	{
		newNetwork->outputNodes[i] = nodeMap[oldNetwork->outputNodes[i]];
	}

	newNetwork->Update();

	return newNetwork;
}

Layer::Layer(NeuralNetwork* neuralNetwork) : neuralNetwork(neuralNetwork)
{
	neuralNetwork->push_back(this);
}

Layer::Layer(NeuralNetwork* neuralNetwork, std::list<Layer*>::const_iterator pos) : neuralNetwork(neuralNetwork)
{
	neuralNetwork->insert(pos, this);
}

Layer::~Layer()
{
	while (!nodes.empty())
	{
		delete nodes.front();
		pop_front();
	}
}

void Layer::DeleteIfEmpty()
{
	if (empty())
	{
		Delete();
	}
}

void Layer::Delete()
{
	neuralNetwork->remove(this);
	delete this;
}

Node::Node(Layer* layer, ActivationFunction* function) : function(function)
{
	SetLayer(layer);
}

Node::Node(Layer* layer, ActivationFunction* function, double bias) : function(function), bias(bias)
{
	SetLayer(layer);
}

Node::~Node()
{
	ClearSynapses();
}

void Node::Update()
{
	outputValue = (*function)(inputValue);

	for (Synapse*& output : outputs)
	{
		output->out->inputValue += (outputValue * output->weight);
	}

	inputValue = bias;
}

void Node::Delete()
{
	if (layer)
	{
		layer->remove(this);
		layer->DeleteIfEmpty();
	}
	delete this;
}

void Node::ClearInputs()
{
	while (!inputs.empty())
	{
		delete inputs.front();
	}
}

void Node::ClearOutputs()
{
	while (!outputs.empty())
	{
		delete outputs.front();
	}
}

void Node::ClearSynapses()
{
	ClearInputs();
	ClearOutputs();
}

void Node::SetLayer(Layer* newLayer)
{
	if (newLayer)
	{
		if (layer)
		{
			layer->remove(this);
		}
		newLayer->push_back(this);
		layer = newLayer;
	}
}

Synapse::Synapse(Node* in, Node* out, double weight) : in(in), out(out), weight(weight)
{
	Link();
}

Synapse::~Synapse()
{
	Unlink();
}

void Synapse::Link()
{
	in->outputs.push_back(this);
	out->inputs.push_back(this);
}

void Synapse::Unlink()
{
	in->outputs.remove(this);
	out->inputs.remove(this);
}

NeuralNetwork* NEATNetworkInterface::ConstructNetworkFromGenes(std::vector<ActivationFunction*>& functions)
{
	NeuralNetwork* newNetwork = new NeuralNetwork(functions);

	std::unordered_map<size_t, Node*> networkNodes;
	for (NodeGene& nodeGene : nodeGenes)
	{
		Node* newNode = new Node(nullptr, nodeGene.activationFunction, nodeGene.bias);
		networkNodes.emplace(nodeGene.nodeID, newNode);
		switch (nodeGene.nodeType)
		{
		case NodeType::INPUT:
			newNetwork->AddInput(newNode);
			break;
		case NodeType::OUTPUT:
			newNetwork->AddOutput(newNode);
			break;
		case NodeType::HIDDEN:
		{
			// Check to see if layer index will place the output layer or out of range
			while (nodeGene.layerIndex < newNetwork->size() - 1)
			{
				// insert layers before the output layer
				new Layer(newNetwork, std::prev(std::prev(newNetwork->end())));
			}
			auto it = newNetwork->begin();
			std::advance(it, nodeGene.layerIndex);
			newNode->SetLayer(*it);
			break;
		}
		default:
			break;
		}
	}
	for (SynapseGene& synapseGene : synapseGenes)
	{
		if (synapseGene.isEnabled)
		{
			try
			{
				new Synapse(networkNodes.at(synapseGene.fromNodeID), networkNodes.at(synapseGene.toNodeID), synapseGene.weight);
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Error: Couldn't find node for synapse creation from NEAT synapse gene\n";
			}
		}
	}
	return newNetwork;
}

void NEATNetworkInterface::ConstructGenesFromNetwork(NeuralNetwork* network)
{
	NeuralNetwork::iterator layeriter = network->begin();
	std::unordered_map<Node*, size_t> nodeMap;
	size_t nodeID = 0;
	size_t layerIndex = 0;
	while (layeriter != network->end())
	{
		Layer* layer = *layeriter;

		for (Node* node : *layer)
		{
			nodeMap.emplace(nodeID, node);
			NodeType nType = layer == network->front() ? NodeType::INPUT : layer == network->back() ? NodeType::OUTPUT : NodeType::HIDDEN;
			NodeGene gene(nodeID,nType,node->function,)
			nodeID++;
		}

		layerIndex++;
	}
}


