#pragma once
#include "NeuralNetwork.h"
#include "raylib.h"
class NetworkVis
{
public:
	NeuralNetwork* network;
	Rectangle drawRec;
	int nodeSize = 10;
	NetworkVis(NeuralNetwork* network, Rectangle drawRec) : network(network), drawRec(drawRec){
		upadateSpaceing();
	}
	~NetworkVis() {};
	void draw() {
		upadateSpaceing();
		for (std::list<Node*> layer : network->network) {
			for (Node* node : layer) {
				Vector2 pos1 = getDrawPos(getNodeId(node));
				for (Synapse* s : node->outputs) {
					Vector2 pos2 = getDrawPos(getNodeId(s->out));
					if (s->weight > 0) {
						DrawLineEx(pos1, pos2, s->weight * drawWeightMult, posSynapseColor);
					}
					else if (s->weight < 0) {
						DrawLineEx(pos1, pos2, abs(s->weight) * drawWeightMult, negSynapseColor);
					}
				}
				Color color = WHITE;
				if (node->outputValue > 0) {
					color.g *= (1 - node->outputValue);
				}
				else
				{
					color.r *= (1 - abs(node->outputValue));
					color.b *= (1 - abs(node->outputValue));
				}
				DrawCircleV(pos1, nodeSize, color);
			}
		}
	}
private:
	int layerSpaceing;
	std::vector<int> nodeSpaceing;
	Color posSynapseColor = RED;
	Color negSynapseColor = BLUE;
	float drawWeightMult = 2;
	void upadateSpaceing() {
		layerSpaceing = drawRec.width / network->network.size();
		nodeSpaceing.clear();
		for (std::list<Node*> layer : network->network) {
			nodeSpaceing.push_back(drawRec.height / layer.size());
		}
	}
	Vector2 getNodeId(Node* node) {
		Vector2 nodeId{0,0};
		bool nodeFound = false;
		std::list<std::list<Node*>>::iterator layerIter = network->network.begin();
		while (!nodeFound && layerIter != network->network.end()) {
			std::list<Node*>::iterator nodeIter = (*layerIter).begin();
			nodeId.y = 0;
			while (!nodeFound && nodeIter != (*layerIter).end()) {
				nodeFound = *nodeIter == node;
				if (!nodeFound) {
					nodeIter++;
					nodeId.y++;
				}
			}
			if (!nodeFound) {
				layerIter++;
				nodeId.x++;
			}
		}
		return nodeId;
	}
	Vector2 getDrawPos(Vector2 pos) {
		pos.y *= nodeSpaceing[pos.x];
		pos.y += drawRec.y - (drawRec.height - nodeSpaceing[pos.x]) / 2;
		pos.x *= layerSpaceing;
		pos.x += drawRec.x - (drawRec.width - layerSpaceing) / 2;
		return pos;
	}
};
