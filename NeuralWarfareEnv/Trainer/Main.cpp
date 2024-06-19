#include <iostream>
#include <chrono>
#include <random>
#include <future>
#include "raylib.h"
#include "NeuralWarfareEngine.h"
#include "NeuralWarfareEnv.h"
#include "NeuralNetwork.h"
#include "ActivationFunctions.h"
#include "RaylibNetworkVis.h"
#include "Trainer.h"
#include "NeuralWarfareTrainers.h"


static void UpdateTrainers(std::vector<Trainer*>& trainers)
{
	for (Trainer* trainer : trainers)
	{
		trainer->Update();
	}
}

int main()
{
	float windowWidth = 1200;
	float windowHeight = 800;

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> dis(1, 2);
	NeuralWarfareEngine eng(gen, Vec2(550, 350));
	Rectangle drawRec{ 50, 50, 1100, 700 };

	AddFunction addfunction;
	TanhFunction tanhFunction;
	SigmoidFunction sigmoidFunction;
	std::vector<ActivationFunction*> functions;
	functions.push_back(&addfunction);
	functions.push_back(&sigmoidFunction);
	functions.push_back(&tanhFunction);

	NeuralNetwork* network = new NeuralNetwork(functions);
	size_t inputSize = NeuralWarfareEnv::MyObservation(eng, nullptr).NNInputSize();
	size_t outputSize = 3;
	for (size_t i = 0; i < inputSize; i++)
	{
		network->AddInput(new Node(nullptr, &addfunction));
	}
	//for (size_t i = 0; i < (inputSize + outputSize) / 2; i++)
	//{
	//	new Node(*std::prev(network->end()), &tanhFunction);
	//}
	//new Layer(network);
	for (size_t i = 0; i < outputSize; i++)
	{
		network->AddOutput(new Node(nullptr, &sigmoidFunction));
	}
	//network->MakeFullyConnected();
	NetworkVis netVis(network, { windowWidth / 2,windowHeight / 2, 1200, 800 });

	GeneticAlgorithmNNTrainer::MyHyperparameters hyperperameters(10, 5, 0.1, 0.2, 0.5, 0.2, 0.05, 1, 0.01, 0.005, 5, 3, addfunction.name);

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<Trainer*> trainers;
	std::uniform_real_distribution<float> spawnXDis(-eng.simSize.x, eng.simSize.x);
	std::uniform_real_distribution<float> spawnYDis(-eng.simSize.y, eng.simSize.y);
	for (size_t i = 0; i < 2; i++)
	{
		envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(100, 1, { spawnXDis(gen),spawnYDis(gen) })));
		trainers.push_back(new GeneticAlgorithmNNTrainer(envs.back(), gen, hyperperameters, NeuralNetwork::Copy(network)));
	}

	InitWindow(windowWidth, windowHeight, "test");

	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();
	float resetTimer = 0;
	std::future<void>* trainerFuture = nullptr;
	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;

		for (size_t i = 0; i < 10; i++)
		{
			resetTimer += 1.0f/60.0f;//deltaTime;

			if (resetTimer > 10)
			{
				eng.Reset();
				resetTimer = 0;
			}
			for (Trainer* trainer : trainers)
			{
				trainer->ObserveEnvironment();
			}

			//delete trainerFuture;
			//trainerFuture = new std::future<void>(std::async(std::launch::async, UpdateTrainers, std::ref(trainers)));
			UpdateTrainers(trainers);
			eng.Update(2);
			//trainerFuture->wait();

			for (Trainer* trainer : trainers)
			{
				trainer->ExecuteAction();
			}
		}

		network = reinterpret_cast<GeneticAlgorithmNNTrainer*>(trainers.front())->masterNetwork;

		BeginDrawing();
		ClearBackground(BLACK);
		if (IsKeyDown(KEY_SPACE))
		{
			netVis.network = network;
			netVis.Draw();
		}
		eng.Draw(drawRec);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		EndDrawing();
	}
}