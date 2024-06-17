#pragma once
#include <string>
#include "Trainer.h"
#include "NeuralWarfareEnv.h"
#include "NeuralNetwork.h"

class TestTrainer : public Trainer
{
public:
	TestTrainer(Environment* env);
	~TestTrainer();
	void Update() override;

private:
};

class StaticNNTrainer : public Trainer
{
public:
	StaticNNTrainer(Environment* env, NeuralNetwork* network);
	~StaticNNTrainer() override;
	void Update() override;

private:
	NeuralNetwork* network;
};

class GeneticAlgorithmNNTrainer : public Trainer
{
public:
	static class MyHyperparameters : Hyperparameters
	{
	public:
		MyHyperparameters(std::string fileName) : newLayerFunction(newLayerFunction)
		{
			Load(fileName);
		}

		MyHyperparameters(
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
			std::string newLayerFunction ) :
			biasMutationRate(biasMutationRate),
			biasMutationMagnitude(biasMutationMagnitude),
			weightMutationRate(weightMutationRate),
			weightMutationMagnitude(weightMutationMagnitude),
			synapseMutationRate(synapseMutationRate),
			newSynapseMagnitude(newSynapseMagnitude),
			nodeMutationRate(nodeMutationRate),
			layerMutationRate(layerMutationRate),
			newLayerSizeAverage(newLayerSizeAverage),
			newLayerSizeRange(newLayerSizeRange),
			newLayerFunction(newLayerFunction)
		{};

		~MyHyperparameters() {};

		/// <summary>
		/// Loads hyperparameters from a file.
		/// </summary>
		/// <param name="fileName">The file from which to load hyperparameters.</param>
		void Load(std::string fileName) override;

		/// <summary>
		/// Saves hyperparameters to a file.
		/// </summary>
		/// <param name="fileName">The file to which to save hyperparameters.</param>
		void Save(std::string fileName) override;

		double biasMutationRate;
		double biasMutationMagnitude;
		double weightMutationRate;
		double weightMutationMagnitude;
		double synapseMutationRate;
		double newSynapseMagnitude;
		double nodeMutationRate;
		double layerMutationRate;
		int newLayerSizeAverage;
		int newLayerSizeRange; 
		std::string newLayerFunction;

	private:

	};

	GeneticAlgorithmNNTrainer(Environment* env, MyHyperparameters hyperparameters, NeuralNetwork* masterNetwork);
	~GeneticAlgorithmNNTrainer() override;
	void Update() override;
private:

	void SetNewLayerFunction();

	static class Agent
	{
	public:
		Agent() {}
		~Agent()
		{
			network->Delete();
		}
		NeuralNetwork* network;
		float fitness = 0;
	private:

	};
	NeuralNetwork* masterNetwork;
	ActivationFunction* newLayerFunction = nullptr;
	std::vector<NeuralNetwork*> networks;
	MyHyperparameters hyperparameters;
};
