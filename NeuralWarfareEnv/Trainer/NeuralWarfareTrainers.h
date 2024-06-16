#pragma once
#include "Trainer.h"
#include "NeuralNetwork.h"

class TestTrainer : public Trainer
{
public:
	TestTrainer(Environment* env);
	~TestTrainer();
	void Think() override;

private:
};

class StaticNNTrainer : public Trainer
{
public:
	StaticNNTrainer(Environment* env, NeuralNetwork* network);
	~StaticNNTrainer();
	void Think() override;

private:
	NeuralNetwork* network;
};
