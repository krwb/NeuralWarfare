#pragma once
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

/// <summary>
/// Represents an activation function used by neural network nodes.
/// </summary>
class ActivationFunction
{
public:
	std::string name; // The name of the activation function.

	/// <summary>
	/// Virtual destructor for ActivationFunction.
	/// </summary>
	virtual ~ActivationFunction() {};

	/// <summary>
	/// Operator to compute the activation function's output for a given input.
	/// </summary>
	/// <param name="in">The input value.</param>
	/// <returns>The output value after applying the activation function.</returns>
	virtual double operator()(double in) = 0;

private:

};

class Layer;
class Node;
class Synapse;



/// <summary>
/// Represents a neural network composed of layers of nodes.
/// </summary>
class NeuralNetwork
{
private:
	std::list<Layer*> layers; // List of layers in the neural network.

public:
	using iterator = std::list<Layer*>::iterator; // Iterator type for accessing layers.
	using const_iterator = std::list<Layer*>::const_iterator; // Const iterator type for accessing layers.

	/// <summary>
	/// Constructs a neural network with specified activation functions for nodes.
	/// </summary>
	/// <param name="functions">Vector of activation functions available for nodes.</param>
	NeuralNetwork(std::vector<ActivationFunction*>& functions);

	/// <summary>
	/// NeuralNetwork destructor.
	/// </summary>
	~NeuralNetwork();

	std::vector<ActivationFunction*> functions; // Available activation functions for nodes.

	/// <summary>
	/// Adds a new node as an input node to the neural network.
	/// </summary>
	/// <param name="newNode">The new input node to add.</param>
	void AddInput(Node* newNode);

	/// <summary>
	/// Adds a new node as an output node to the neural network.
	/// </summary>
	/// <param name="newNode">The new output node to add.</param>
	void AddOutput(Node* newNode);

	/// <summary>
	/// Evaluates the neural network with given input values.
	/// </summary>
	/// <param name="inputValues">Vector of input values to the neural network.</param>
	/// <returns>Vector of output values from the neural network.</returns>
	std::vector<double> Evaluate(std::vector<double> inputValues);

	/// <summary>
	/// Updates the neural network (e.g., learning, adaptation).
	/// </summary>
	void Update();

	/// <summary>
	/// Ensures all nodes in the neural network are fully connected.
	/// </summary>
	void MakeFullyConnected();

	/// <summary>
	/// Cleans up all synapses in the neural network.
	/// </summary>
	void CleanSynapses();

	/// <summary>
	/// Cleans up all nodes in the neural network.
	/// </summary>
	void CleanNodes();

	/// <summary>
	/// Cleans up the entire neural network.
	/// </summary>
	void Clean();

	/// <summary>
	/// Deletes the entire neural network.
	/// </summary>
	void Delete();

	/// <summary>
	/// Creates a deep copy of an existing neural network.
	/// </summary>
	/// <param name="oldNetwork">The neural network to copy.</param>
	/// <returns>A new instance of the copied neural network.</returns>
	static NeuralNetwork* Copy(NeuralNetwork* oldNetwork);

	/// <summary>
	/// Retrieves a binary representation of the neural network.
	/// </summary>
	/// <param name="network">The neural network to convert to binary.</param>
	/// <returns>Vector of characters representing the binary data of the neural network.</returns>
	static std::vector<char> GetBin(const NeuralNetwork& network);

	/// <summary>
	/// Saves the neural network to a file.
	/// </summary>
	/// <param name="network">The neural network to save.</param>
	/// <param name="filename">The name of the file to save to.</param>
	static void Save(const NeuralNetwork& network, std::string filename);

	/// <summary>
	/// Constructs a neural network from its binary representation.
	/// </summary>
	/// <param name="data">Vector of characters representing the binary data.</param>
	/// <param name="avalableFunctions">Available activation functions for nodes.</param>
	/// <param name="offset">Offset within the binary data to start constructing from.</param>
	/// <returns>A new instance of the constructed neural network.</returns>
	static NeuralNetwork* MakeFromBin(std::vector<char>& data, std::vector<ActivationFunction*>& avalableFunctions, size_t& offset);

	/// <summary>
	/// Loads a neural network from a file.
	/// </summary>
	/// <param name="functions">Available activation functions for nodes.</param>
	/// <param name="filename">The name of the file to load the neural network from.</param>
	/// <returns>A new instance of the loaded neural network.</returns>
	static NeuralNetwork* Load(std::vector<ActivationFunction*>& functions, std::string filename);

	// List interface methods:

	iterator begin() { return layers.begin(); }
	const_iterator begin() const { return layers.begin(); }
	iterator end() { return layers.end(); }
	const_iterator end() const { return layers.end(); }

	bool empty() const { return layers.empty(); }
	size_t size() const { return layers.size(); }

	Layer*& front() { return layers.front(); }
	const Layer* front() const { return layers.front(); }
	Layer*& back() { return layers.back(); }
	const Layer* back() const { return layers.back(); }

	void push_front(Layer* value) { layers.push_front(value); }
	void pop_front() { layers.pop_front(); }

	void push_back(Layer* value) { layers.push_back(value); }
	void pop_back() { layers.pop_back(); }

	void clear() { layers.clear(); }

	iterator insert(const_iterator pos, Layer* value) { return layers.insert(pos, value); }
	iterator insert(const_iterator pos, size_t count, Layer* value) { return layers.insert(pos, count, value); }

	void resize(size_t count) { layers.resize(count); }
	void resize(size_t count, Layer* value) { layers.resize(count, value); }

	void remove(Layer* value) { layers.remove(value); }
	template <typename Pr>
	void remove_if(Pr p) { layers.remove_if(p); }

	void reverse() { layers.reverse(); }

private:
	std::vector<Node*> inputNodes; // Array of input nodes in the neural network.
	std::vector<Node*> outputNodes; // Array of output nodes in the neural network.


	/// <summary>
	/// Appends a value to a vector of characters representing binary data.
	/// </summary>
	/// <typeparam name="T">Type of value to append.</typeparam>
	/// <param name="data">Vector of characters representing binary data.</param>
	/// <param name="value">Value to append.</param>
	template <typename T>
	static void AppendToData(std::vector<char>& data, const T& value)
	{
		size_t size = sizeof(T);
		const char* ptr = reinterpret_cast<const char*>(&value);
		data.insert(data.end(), ptr, ptr + size);
	}

	/// <summary>
	/// Appends a string to a vector of characters representing binary data.
	/// </summary>
	/// <param name="data">Vector of characters representing binary data.</param>
	/// <param name="value">String to append.</param>
	static void AppendToData(std::vector<char>& data, const std::string& value)
	{
		size_t size = value.size();
		AppendToData(data, size);
		data.insert(data.end(), value.begin(), value.end());
	}

	/// <summary>
	/// Extracts a value from a vector of characters representing binary data.
	/// </summary>
	/// <typeparam name="T">Type of value to extract.</typeparam>
	/// <param name="data">Vector of characters representing binary data.</param>
	/// <param name="offset">Offset within the binary data to start extracting from.</param>
	/// <param name="value">Extracted value.</param>
	template <typename T>
	static void ExtractFromData(const std::vector<char>& data, size_t& offset, T& value)
	{
		std::memcpy(&value, data.data() + offset, sizeof(T));
		offset += sizeof(T);
	}
	
	/// <summary>
	/// Extracts a string from a vector of characters representing binary data.
	/// </summary>
	/// <param name="data">Vector of characters representing binary data.</param>
	/// <param name="offset">Offset within the binary data to start extracting from.</param>
	/// <param name="value">Extracted string.</param>
	static void ExtractFromData(const std::vector<char>& data, size_t& offset, std::string& value)
	{
		size_t size;
		ExtractFromData(data, offset, size);
		value.assign(data.data() + offset, size);
		offset += size;
	}

};

class Layer
{
private:
	std::list<Node*> nodes; // List of nodes within the layer.
	NeuralNetwork* neuralNetwork; // Pointer to the parent neural network.

public:
	using iterator = std::list<Node*>::iterator; // Iterator type for accessing nodes.
	using const_iterator = std::list<Node*>::const_iterator; // Const iterator type for accessing nodes.

	/// <summary>
	/// Constructs a layer within a neural network.
	/// </summary>
	/// <param name="neuralNetwork">Pointer to the parent neural network.</param>
	Layer(NeuralNetwork* neuralNetwork);

	/// <summary>
	/// Constructs a layer within a neural network, inserting it at a specified position.
	/// </summary>
	/// <param name="neuralNetwork">Pointer to the parent neural network.</param>
	/// <param name="pos">Position to insert the layer within the neural network.</param>
	Layer(NeuralNetwork* neuralNetwork, NeuralNetwork::const_iterator pos);

	/// <summary>
	/// Layer destructor.
	/// </summary>
	~Layer();

	/// <summary>
	/// Deletes the layer if it contains no nodes.
	/// </summary>
	void DeleteIfEmpty();

	/// <summary>
	/// Deletes the layer, including all nodes it contains.
	/// </summary>
	void Delete();

	// List interface methods:

	iterator begin() { return nodes.begin(); }
	const_iterator begin() const { return nodes.begin(); }
	iterator end() { return nodes.end(); }
	const_iterator end() const { return nodes.end(); }

	bool empty() const { return nodes.empty(); }
	size_t size() const { return nodes.size(); }

	Node*& front() { return nodes.front(); }
	const Node* front() const { return nodes.front(); }
	Node*& back() { return nodes.back(); }
	const Node* back() const { return nodes.back(); }

	void push_front(Node* value) { nodes.push_front(value); }
	void pop_front() { nodes.pop_front(); }

	void push_back(Node* value) { nodes.push_back(value); }
	void pop_back() { nodes.pop_back(); }

	void clear() { nodes.clear(); }

	iterator insert(const_iterator pos, Node* value) { return nodes.insert(pos, value); }
	iterator insert(const_iterator pos, size_t count, Node* value) { return nodes.insert(pos, count, value); }
	
	void resize(size_t count) { nodes.resize(count); }
	void resize(size_t count, Node* value) { nodes.resize(count, value); }

	void remove(Node* value) { nodes.remove(value); }
	template <typename Pr>
	void remove_if(Pr p) { nodes.remove_if(p); }

	void reverse() { nodes.reverse(); };

private:
};

/// <summary>
/// Represents a node within a neural network layer.
/// </summary>
class Node
{
public:
	/// <summary>
	/// Constructs a node within a specified layer and with a given activation function.
	/// </summary>
	/// <param name="layer">The layer to which the node belongs.</param>
	/// <param name="function">The activation function used by the node.</param>
	Node(Layer* layer, ActivationFunction* function);

	/// <summary>
	/// Constructs a node within a specified layer, with a given activation function and bias.
	/// </summary>
	/// <param name="layer">The layer to which the node belongs.</param>
	/// <param name="function">The activation function used by the node.</param>
	/// <param name="bias">The bias value of the node.</param>
	Node(Layer* layer, ActivationFunction* function, double bias);

	/// <summary>
	/// Node destructor.
	/// </summary>
	~Node();

	double bias = 0; // Bias value of the node.
	double inputValue = bias; // Input value to the node.
	double outputValue = 0; // Output value from the node.

	ActivationFunction* function; // Activation function used by the node.

	/// <summary>
	/// Updates the state of the node based on its inputs and current parameters.
	/// </summary>
	void Update();

	/// <summary>
	/// Clears all input connections (synapses) to the node.
	/// </summary>
	void ClearInputs();

	/// <summary>
	/// Clears all output connections (synapses) from the node.
	/// </summary>
	void ClearOutputs();

	/// <summary>
	/// Clears all synapses (both input and output) connected to the node.
	/// </summary>
	void ClearSynapses();

	/// <summary>
	/// Sets the layer to which the node belongs.
	/// </summary>
	/// <param name="newLayer">The new layer to assign to the node.</param>
	void SetLayer(Layer* newLayer);

	/// <summary>
	/// Deletes the node and all its associated resources.
	/// </summary>
	void Delete();

	std::list<Synapse*> inputs; // List of input synapses (connections) to the node.
	std::list<Synapse*> outputs; // List of output synapses (connections) from the node.

private:
	Layer* layer; // Pointer to the layer that contains the node.
};

/// <summary>
/// Represents a synapse (connection) between two nodes in a neural network.
/// </summary>
class Synapse
{
public:
	/// <summary>
	/// Constructs a synapse between an input node and an output node with a specified weight.
	/// </summary>
	/// <param name="in">The input node of the synapse.</param>
	/// <param name="out">The output node of the synapse.</param>
	/// <param name="weight">The weight (strength) of the synapse.</param>
	Synapse(Node* in, Node* out, double weight);

	/// <summary>
	/// Synapse destructor.
	/// </summary>
	~Synapse();

	Node* in; // Pointer to the input node of the synapse.
	Node* out; // Pointer to the output node of the synapse.
	double weight; // Weight (strength) of the synapse.

	/// <summary>
	/// Establishes (links) the connection between the input and output nodes.
	/// </summary>
	void Link();

	/// <summary>
	/// Removes (unlinks) the connection between the input and output nodes.
	/// </summary>
	void Unlink();

private:

};

enum class NodeType
{
	INPUT,
	OUTPUT,
	HIDDEN
};

class NodeGene 
{
public:
	NodeGene(size_t nodeID, NodeType type, ActivationFunction* function, size_t layerIndex, double bias)
		: nodeID(nodeID), nodeType(type), activationFunction(function), layerIndex(layerIndex), bias(bias) {
	}
	NodeType nodeType;
	ActivationFunction* activationFunction;
	double bias;

	size_t nodeID;
	size_t layerIndex;
};

class SynapseGene 
{
public:
	SynapseGene(int innovation, size_t fromNode, size_t toNode, double w, bool enabled)
		: innovationNumber(innovation), fromNodeID(fromNode), toNodeID(toNode), weight(w), isEnabled(enabled) 
	{}

	int innovationNumber;
	size_t fromNodeID;
	size_t toNodeID;
	double weight;
	bool isEnabled;
};

class NEATNetworkInterface
{
	std::vector<NodeGene> nodeGenes;
	std::vector<SynapseGene> synapseGenes;
public:
	NEATNetworkInterface() {};
	~NEATNetworkInterface() {};


	void AddNodeGene(const NodeGene& nodeGene) {
		nodeGenes.push_back(nodeGene);
	}

	void AddSynapseGene(const SynapseGene& synapseGene) {
		synapseGenes.push_back(synapseGene);
	}

	NeuralNetwork* ConstructNetworkFromGenes(std::vector<ActivationFunction*>& functions);

	void ConstructGenesFromNetwork(NeuralNetwork* network);
private:
};