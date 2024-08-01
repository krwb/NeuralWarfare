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

std::string MakeFilename(const std::string& filename, const std::string& extension) {
	std::string correctExtension = (extension[0] == '.') ? extension : "." + extension;
	size_t lastDot = filename.find_last_of('.');
	std::string baseFilename = (lastDot != std::string::npos) ? filename.substr(0, lastDot) : filename;
	return baseFilename + correctExtension;
}

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
	size_t outputSize = NeuralWarfareEnv::MyAction(0).NNOutputSize();
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
	size_t netVisI = 0;
	size_t stepsPerFrame = 1;
	GeneticAlgorithmNNTrainer::MyHyperparameters hyperperameters("hyperperameters.xml");
	
	//network->Delete();
	//network = NeuralNetwork::Load(functions, "net1.bin");

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<Trainer*> trainers;
	std::uniform_real_distribution<float> spawnXDis(-eng.simSize.x, eng.simSize.x);
	std::uniform_real_distribution<float> spawnYDis(-eng.simSize.y, eng.simSize.y);


	size_t trainerCount = 2;

	{
		size_t i = 0;

		envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(200, 2, { -static_cast<float>(cos(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.x / 2.0f, -static_cast<float>(sin(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.y / 2.0f })));
		trainers.push_back(new GeneticAlgorithmNNTrainer(envs.back(), gen, hyperperameters, NeuralNetwork::Load(functions,MakeFilename("red0", "bin"))));
		i++;
		envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(200, 2, { -static_cast<float>(cos(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.x / 2.0f, -static_cast<float>(sin(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.y / 2.0f })));
		trainers.push_back(new GeneticAlgorithmNNTrainer(envs.back(), gen, hyperperameters, NeuralNetwork::Load(functions, MakeFilename("test", "bin"))));

	}

	//for (size_t i = 0; i < trainerCount; i++)
	//{
	//	envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(200, 2, { -static_cast<float>(cos(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.x / 2.0f, -static_cast<float>(sin(std::numbers::pi * (static_cast<double>(i) / static_cast<double>(trainerCount) * 2.0f))) * eng.simSize.y / 2.0f })));
	//	trainers.push_back(new GeneticAlgorithmNNTrainer(envs.back(), gen, hyperperameters, NeuralNetwork::Copy(network)));
	//}

	InitWindow(windowWidth, windowHeight, "test");
	SetTargetFPS(60);

	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();
	bool drawEng = true;
	float resetTimer = 0;
	std::future<void>* trainerFuture = nullptr;
	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;

		for (size_t i = 0; i < stepsPerFrame; i++)
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

			delete trainerFuture;
			trainerFuture = new std::future<void>(std::async(std::launch::async, UpdateTrainers, std::ref(trainers)));
			eng.Update(4);
			trainerFuture->wait();

			for (Trainer* trainer : trainers)
			{
				trainer->ExecuteAction();
			}
		}

		GeneticAlgorithmNNTrainer* selectedTrainer = static_cast<GeneticAlgorithmNNTrainer*>(trainers[netVisI]);

		BeginDrawing();
		ClearBackground(BLACK);
		if (drawEng)
		{
			eng.Draw(drawRec);
		}

		if (IsKeyDown(KEY_SPACE))
		{
			netVis.network = selectedTrainer->masterNetwork;
			netVis.Draw();
			DrawCircle(windowWidth - 15, 15, 10, NeuralWarfareEngine::GenerateTeamColor(netVisI));
		}
		if (IsKeyPressed(KEY_T))
		{
			netVisI = netVisI > 0 ? netVisI - 1 : trainers.size() - 1;
		}
		if (IsKeyPressed(KEY_Y))
		{
			netVisI = netVisI < trainers.size() - 1 ? netVisI + 1 : 0;
		}
		if (IsKeyPressed(KEY_S))
		{
			std::string filename;
			std::cout << "Filename : ";
			std::cin >> filename;
			NeuralNetwork::Save(*selectedTrainer->masterNetwork, MakeFilename(filename, "bin"));
		}
		if (IsKeyPressed(KEY_MINUS))
		{
			stepsPerFrame = stepsPerFrame > 0 ? stepsPerFrame - 1 : 0;
		}
		if (IsKeyPressed(KEY_EQUAL))
		{
			stepsPerFrame++;
		}
		if (IsKeyPressed(KEY_E))
		{
			drawEng = !drawEng;
		}
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		EndDrawing();
	}
}