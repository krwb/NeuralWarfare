#include <iostream>
#include <chrono>
#include <random>
#include <future>
#include "raylib.h"
#include "NeuralNetwork.h"
#include "ActivationFunctions.h"
#include "RaylibNetworkVis.h"
#include "Trainer.h"
#include "NeuralWarfareTrainers.h"
#include "NeuralWarfareEnv.h"
#include "NeuralWarfareEngine.h"

void UpdateTrainers(std::vector<TestTrainer>& trainers)
{
	for (Trainer& trainer : trainers)
	{
		trainer.Update();
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
	std::vector<ActivationFunction*> functions;
	functions.push_back(&addfunction);

	NeuralNetwork* network = NeuralNetwork::Load(functions, "test.bin");
	NetworkVis netVis(network, { windowWidth / 2,windowHeight / 2, 1200, 800 });

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<TestTrainer> trainers;
	std::uniform_real_distribution<float> spawnXDis(-eng.simSize.x, eng.simSize.x);
	std::uniform_real_distribution<float> spawnYDis(-eng.simSize.y, eng.simSize.y);
	for (size_t i = 0; i < 10; i++)
	{
		envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(100, 2, { spawnXDis(gen),spawnYDis(gen) })));
		trainers.emplace_back(envs.back());
	}

	InitWindow(windowWidth, windowHeight, "test");
	SetTargetFPS(60);

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

		resetTimer += deltaTime;

		if (resetTimer > 5)
		{
			eng.Reset();
			resetTimer = 0;
		}

		for (size_t i = 0; i < 5; i++)
		{
			for (Trainer& trainer : trainers)
			{
				trainer.ObserveEnvironment();
			}

			delete trainerFuture;
			trainerFuture = new std::future<void>(std::async(std::launch::async, UpdateTrainers, std::ref(trainers)));
			eng.Update(2);
			trainerFuture->wait();

			for (Trainer& trainer : trainers)
			{
				trainer.ExecuteAction();
			}
		}


		BeginDrawing();
		ClearBackground(BLACK);
		netVis.Draw();
		eng.Draw(drawRec);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		EndDrawing();
	}
}