#pragma once
#include <unordered_map>
#include "raylib.h"
#include "NeuralNetwork.h"

/// <summary>
/// Class for visualizing a neural network.
/// </summary>
class NetworkVis
{
public:
	NeuralNetwork* network; // Pointer to the neural network to be visualized.
	Rectangle drawRec; // Rectangle defining the drawing area for the visualization.

	/// <summary>
	/// Constructs a network visualization object.
	/// </summary>
	/// <param name="network">Pointer to the neural network to be visualized.</param>
	/// <param name="drawRec">Rectangle defining the drawing area for the visualization.</param>
	NetworkVis(NeuralNetwork* network, Rectangle drawRec) : network(network), drawRec(drawRec) {}

	/// <summary>
	/// Destructor for NetworkVis.
	/// </summary>
	~NetworkVis() {}

	/// <summary>
	/// Draws the neural network visualization.
	/// </summary>
	void Draw()
	{
		if (network) {
			UpdatePosMap();
			DrawNodesAndSynapses();
		}
	}

private:
	int layerSpaceing = 0; // Space between layers.
	std::vector<int> nodeSpaceing; // Space between nodes within layers.
	int nodeSize = 10; // Size of the nodes.
	Color posSynapseColor = RED; // Color for positive-weight synapses.
	Color negSynapseColor = BLUE; // Color for negative-weight synapses.
	float drawWeightMultiplier = 2; // Multiplier for drawing synapse weights.

	std::unordered_map<Node*, Vector2> posMap; ///< Map of node positions for drawing.

	/// <summary>
	/// Updates the spacing between layers and nodes.
	/// </summary>
	void UpadateSpaceing()
	{
		layerSpaceing = drawRec.width / network->size();
		nodeSpaceing.clear();
		for (Layer* layer : *network) {
			nodeSpaceing.push_back(drawRec.height / layer->size());
		}
	}

	/// <summary>
	/// Calculates the drawing position of a node.
	/// </summary>
	/// <param name="pos">The logical position of the node within the network.</param>
	/// <returns>The screen position for drawing the node.</returns>
	Vector2 GetDrawPos(Vector2 pos)
	{
		pos.y *= nodeSpaceing[pos.x];
		pos.y += drawRec.y - (drawRec.height - nodeSpaceing[pos.x]) / 2;
		pos.x *= layerSpaceing;
		pos.x += drawRec.x - (drawRec.width - layerSpaceing) / 2;
		return pos;
	}

	/// <summary>
	/// Updates the position map for all nodes in the network.
	/// </summary>
	void UpdatePosMap()
	{
		UpadateSpaceing();
		posMap.clear();
		Vector2 netPos{ 0,0 };
		NeuralNetwork::iterator layerIter = network->begin();
		while (layerIter != network->end()) {
			Layer::iterator nodeIter = (*layerIter)->begin();
			netPos.y = 0;
			while (nodeIter != (*layerIter)->end()) {
				posMap.insert({ *nodeIter, GetDrawPos(netPos) });
				nodeIter++;
				netPos.y++;
			}
			layerIter++;
			netPos.x++;
		}
	}

	/// <summary>
	/// Draws all nodes and synapses in the network.
	/// </summary>
	void DrawNodesAndSynapses() const {
		for (const auto& layer : *network) {
			for (const auto& node : *layer) {
				Vector2 pos1 = posMap.at(node);
				DrawSynapses(node, pos1);
				DrawNode(node, pos1);
			}
		}
	}

	/// <summary>
	/// Draws all synapses for a given node.
	/// </summary>
	/// <param name="node">The node whose synapses are to be drawn.</param>
	/// <param name="pos1">The screen position of the node.</param>
	void DrawSynapses(const Node* node, const Vector2& pos1) const {
		for (const auto& synapse : node->outputs) {
			Vector2 pos2 = posMap.at(synapse->out);
			Color synapseColor = (synapse->weight > 0) ? posSynapseColor : negSynapseColor;
			float lineThickness = abs(synapse->weight) * drawWeightMultiplier;
			DrawLineEx(pos1, pos2, lineThickness > 1 ? lineThickness : 1, synapseColor);
		}
	}

	/// <summary>
	/// Draws a single node.
	/// </summary>
	/// <param name="node">The node to be drawn.</param>
	/// <param name="pos1">The screen position of the node.</param>
	void DrawNode(const Node* node, const Vector2& pos1) const {
		Color color = WHITE;
		if (node->outputValue > 0) {
			color.g *= (1 - node->outputValue);
		}
		else {
			color.r *= (1 - abs(node->outputValue));
			color.b *= (1 - abs(node->outputValue));
		}
		DrawCircleV(pos1, nodeSize, color);
	}
};
