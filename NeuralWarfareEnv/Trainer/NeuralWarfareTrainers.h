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
			size_t topAgentCount,
			size_t mutationCouunt,
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
			topAgentCount(topAgentCount),
			mutationCouunt(mutationCouunt),
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

		size_t topAgentCount;
		size_t mutationCouunt;
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

	GeneticAlgorithmNNTrainer(Environment* env, std::mt19937& gen, MyHyperparameters hyperparameters, NeuralNetwork* masterNetwork);
	~GeneticAlgorithmNNTrainer() override;
	void Update() override;

	NeuralNetwork* masterNetwork;

private:

	void SetNewLayerFunction();

	void Evolve();

	static class Agent
	{
	public:
		Agent(NeuralNetwork* network);
		~Agent();
		NeuralNetwork* network;
		void SetNetwork(NeuralNetwork* newNetwork);
		float fitness = 0;
	private:

	};

	ActivationFunction* newLayerFunction = nullptr;
	std::vector<Agent*> agents;
	std::mt19937& gen;
	MyHyperparameters hyperparameters;
};
