#pragma once
#include "NeuralNetwork.h"
#include <random>

template<typename T>
typename std::list<T>::iterator getRandomElement(typename std::list<T>::iterator start, typename std::list<T>::iterator end, std::mt19937& gen) {
	if (start == end) {
		return end;
	}
	std::uniform_int_distribution<size_t> distribution(0, std::distance(start, end) - 1);
	size_t randomIndex = distribution(gen);
	std::advance(start, randomIndex);
	return start;
}

Layer* getRandomLayer(NeuralNetwork::iterator startLayer, NeuralNetwork::iterator endLayer, std::mt19937& gen) {
	NeuralNetwork::iterator randomLayer = getRandomElement<Layer*>(startLayer, endLayer, gen);
	return randomLayer != endLayer ? *randomLayer : nullptr;
}

Node* getRandomNode(NeuralNetwork::iterator startLayer, NeuralNetwork::iterator endLayer, std::mt19937& gen) {
	Layer* randomLayer = getRandomLayer(startLayer, endLayer, gen);
	Layer::iterator randomNode = getRandomElement<Node*>(randomLayer->begin(), randomLayer->end(), gen);
	return randomNode != randomLayer->end() ? *randomNode : nullptr;
}

Synapse* getRandomSynapse(NeuralNetwork::iterator startLayer, NeuralNetwork::iterator endLayer, std::mt19937& gen) {
	Node* randomNode = getRandomNode(startLayer, std::prev(endLayer), gen);
	std::list<Synapse*>::iterator randomSynapse = getRandomElement<Synapse*>(randomNode->outputs.begin(), randomNode->outputs.end(), gen);
	return randomSynapse != randomNode->outputs.end() ? *randomSynapse : nullptr;
}

void SimpleMutate(
	NeuralNetwork* net, std::mt19937& gen,
	double biasMutationRate,
	double biasMutationMagnitude,
	double weightMutationRate,
	double weightMutationMagnitude,
	double synapseMutationRate,
	double newSynapseMagnitude,
	double nodeMutationRate,
	double layerMutationRate,
	int newLayerSizeAverage,
	int newLayerSizeRange,
	ActivationFunction* newLayerFunction
) {
	std::uniform_real_distribution<double> dis(0, 1);
	if (dis(gen) < layerMutationRate) {
		std::uniform_int_distribution<int> d(0, 1);
		if (d(gen) == 1) {
			std::uniform_int_distribution<int> sizeDif(-newLayerSizeRange, newLayerSizeRange);
			NeuralNetwork::iterator randomLayerIter = getRandomElement<Layer*>(std::next(net->begin()), std::prev(net->end()), gen);
			Layer* newLayer = new Layer(net, randomLayerIter);
			size_t newLayerSize = newLayerSizeAverage + sizeDif(gen);
			for (size_t i = 0; i < newLayerSize; i++) {
				new Node(newLayer, newLayerFunction);
			}
		}
		else if(net->size() > 2){
			Layer* randomLayer = getRandomLayer(std::next(net->begin()), std::prev(net->end()), gen);
			if (randomLayer)
			{
				randomLayer->Delete();
			}
		}
	}
	NeuralNetwork::iterator layerIter = net->begin();
	NeuralNetwork::iterator outputLayerIter = std::prev(net->end());
	while (layerIter != net->end()) {
		if (layerIter != net->begin() && layerIter != outputLayerIter && dis(gen) < nodeMutationRate) {
			std::uniform_int_distribution<int> d(0, 1);
			if (d(gen) == 1) {
				if ((*layerIter)->size() > 1) {
					Layer::iterator delNodeIter = getRandomElement<Node*>((*layerIter)->begin(), (*layerIter)->end(), gen);
					if (delNodeIter != (*layerIter)->end())
					{
						(*delNodeIter)->Delete();
					}
				}
			}
			else {
				Layer::iterator functionNodeIter = getRandomElement<Node*>((*layerIter)->begin(), (*layerIter)->end(), gen);
				new Node(*layerIter, newLayerFunction);
			}
		}
		Layer::iterator nodeIter = (*layerIter)->begin();
		while (nodeIter != (*layerIter)->end()) {
			if (dis(gen) < biasMutationRate) {
				std::uniform_real_distribution<double> Mag(-biasMutationMagnitude, biasMutationMagnitude);
				(*nodeIter)->bias += Mag(gen);
			}
			if (layerIter != std::prev(net->end()) && dis(gen) < synapseMutationRate) {
				std::uniform_int_distribution<int> d(0, 1);
				if (d(gen) == 1) {
					if ((*nodeIter)->outputs.size() > 0) {
						std::list<Synapse*>::iterator delSynapseIter = getRandomElement<Synapse*>((*nodeIter)->outputs.begin(), (*nodeIter)->outputs.end(), gen);
						delete* delSynapseIter;
					}
				}
				else {
					std::uniform_real_distribution<double> Mag(-newSynapseMagnitude, newSynapseMagnitude);
					Node* node1 = *nodeIter;
					NeuralNetwork::iterator outLayer = std::next(layerIter);
					Node* node2 = getRandomNode(outLayer, net->end(), gen);
					new Synapse(node1, node2, Mag(gen));
				}
			}
			std::list<Synapse*>::iterator synapseIter = (*nodeIter)->outputs.begin();
			while (synapseIter != (*nodeIter)->outputs.end()) {
				if (dis(gen) < weightMutationRate) {
					std::uniform_real_distribution<double> Mag(-weightMutationMagnitude, weightMutationMagnitude);
					(*synapseIter)->weight += Mag(gen);
				}
				synapseIter++;
			}
			nodeIter++;
		}
		layerIter++;
	}
	net->Clean();
}
