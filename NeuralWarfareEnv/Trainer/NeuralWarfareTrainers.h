#pragma once
#include "Trainer.h"
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
	~StaticNNTrainer();
	void Update() override;

private:
	NeuralNetwork* network;
};
