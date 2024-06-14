#pragma once
#include <list>
#include <vector>
#include <functional>
#include <string>

class ActivationFunction
{
public:
	std::string name;
	virtual ~ActivationFunction() {};
	virtual double operator()(double in) = 0;
private:

};

class Node;
using Layer = std::list<Node*>;

//class Layer
//{
//private:
//	std::list<Node*> nodes;
//	
//public:
//	Layer();
//	~Layer();
//
//	typename std::list<Node*>::iterator begin() { return nodes.begin(); }
//	typename std::list<Node*>::const_iterator begin() const { return nodes.begin(); }
//	typename std::list<Node*>::iterator end() { return nodes.end(); }
//	typename std::list<Node*>::const_iterator end() const { return nodes.end(); }
//
//	bool empty() const { return nodes.empty(); }
//	size_t size() const { return nodes.size(); }
//
//	Node*& front() { return nodes.front(); }
//	const Node* front() const { return nodes.front(); }
//	Node*& back() { return nodes.back(); }
//	const Node* back() const { return nodes.back(); }
//
//	void push_front(Node* value) { nodes.push_front(value); }
//	void pop_front() { nodes.pop_front(); }
//
//	void push_back(Node* value) { nodes.push_back(value); }
//	void pop_back() { nodes.pop_back(); }
//
//	void clear() { nodes.clear(); }
//
//	std::list<Node*>::iterator insert(std::list<Node*>::const_iterator pos, Node* value) {
//		return nodes.insert(pos, value);
//	}
//
//	std::list<Node*>::iterator insert(std::list<Node*>::const_iterator pos, size_t count, Node* value) {
//		return nodes.insert(pos, count, value);
//	}
//
//	std::list<Node*>::iterator erase(std::list<Node*>::const_iterator pos) {
//		return nodes.erase(pos);
//	}
//
//	std::list<Node*>::iterator erase(std::list<Node*>::const_iterator first, std::list<Node*>::const_iterator last) {
//		return nodes.erase(first, last);
//	}
//
//	void resize(size_t count) {
//		nodes.resize(count);
//	}
//
//	void resize(size_t count, Node* value) {
//		nodes.resize(count, value);
//	}
//
//	void remove(Node* value) {
//		nodes.remove(value);
//	}
//
//	template <typename Pr>
//	void remove_if(Pr p) {
//		nodes.remove_if(p);
//	}
//
//	void reverse() {
//		nodes.reverse();
//	}
//private:
//
//};


class Synapse
{
public:
	Synapse(Node* in, Node* out, double weight);
	~Synapse();

	Node* in;
	Node* out;
	double weight;

	void Link();

	void Unlink();

private:

};

class Node
{
public:
	Node(ActivationFunction* function, Layer* layer);
	~Node();

	double bias = 0;
	double inputValue = bias;
	double outputValue = 0;

	ActivationFunction* function;

	void Update();

	std::list<Synapse*> inputs;
	std::list<Synapse*> outputs;

	void ClearInputs();
	void ClearOutputs();
	void ClearSynapses();
	
	void SetLayer(Layer* newLayer);
private:
	Layer* layer;
};

class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();


	void AddInput(Node* newNode);
	void AddOutput(Node* newNode);

	std::vector<double> Evaluate(std::vector<double> inputValues);
	void Update();
	
	void MakeFullyConnected();
	std::list<Layer> network;

private:
	std::vector<Node*> inputNodes;
	std::vector<Node*> outputNodes;
};