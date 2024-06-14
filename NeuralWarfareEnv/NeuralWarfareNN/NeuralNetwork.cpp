#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork()
{
	network.resize(2);
}

NeuralNetwork::~NeuralNetwork()
{
	while (!network.empty())
	{
		while (!network.front().empty())
		{
			delete network.front().front();
		}
		network.pop_front();
	}
}


void NeuralNetwork::AddInput(Node* newNode)
{
	inputNodes.push_back(newNode);
	newNode->SetLayer(&network.front());
}

void NeuralNetwork::AddOutput(Node* newNode)
{
	outputNodes.push_back(newNode);
	newNode->SetLayer(&network.back());
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
	for (Layer& layer : network)
	{
		for (Node* node : layer)
		{
			node->Update();
		}
	}
}

void NeuralNetwork::MakeFullyConnected()
{
	std::list<Layer>::iterator layerIter = network.begin();;
	while (layerIter != std::prev(network.end()))
	{
		for (Node* nodeA : *layerIter)
		{
			for (Node* nodeB : *std::next(layerIter))
			{
				new Synapse(nodeA, nodeB, 1);
			}
		}
		layerIter++;
	}
};


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


Node::Node(ActivationFunction* function, Layer* layer) : function(function)
{
	SetLayer(layer);
}

Node::~Node()
{
	if (layer)
	{
		layer->remove(this);
	}
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

void Node::ClearInputs()
{
	while (!inputs.empty())
	{
		delete inputs.front();
	}
}

void Node::ClearOutputs()
{
	while (!inputs.empty())
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
