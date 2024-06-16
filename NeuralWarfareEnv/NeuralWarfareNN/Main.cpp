#include <iostream>
#include <random>
#include <chrono>
#include "raylib.h"
#include "NeuralNetwork.h"
#include "RaylibNetworkVis.h"
#include "ActivationFunctions.h"
#include "SimpleMutate.h"



int main()
{
	float windowWidth = 1200;
	float windowHeight = 800;

	std::random_device rd;
	std::mt19937 gen(12345);
	std::uniform_real_distribution<double> dis(-1, 1);

	AddFunction addfunction;
	std::vector<ActivationFunction*> functions;
	functions.push_back(&addfunction);

	NeuralNetwork* network = new NeuralNetwork(functions);
	NeuralNetwork* coppiedNetwork = nullptr;

	for (size_t i = 0; i < 5; i++)
	{
		network->AddInput(new Node(nullptr, &addfunction));
	}
	new Layer(network);
	for (size_t i = 0; i < 4; i++)
	{
		new Node(*std::prev(std::prev(network->end())), &addfunction);
	}
	for (size_t i = 0; i < 3; i++)
	{
		network->AddOutput(new Node(nullptr, &addfunction));
	}

	network->MakeFullyConnected();

	NetworkVis netVis(network, { windowWidth / 2,windowHeight / 2, 1200, 800 });

	InitWindow(windowWidth, windowHeight, "test");


	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();

	bool running = false;

	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;
		
		if (running)
		{
			SimpleMutate(network, gen, 0.1, 0.2, 0.5, 0.2, 0.05, 1, 0.01, 0.005, 5, 3, &addfunction);
		}

		if (IsKeyDown(KEY_BACKSPACE) && network->size() > 2)
		{
			getRandomNode(std::next(network->begin()), std::prev(network->end()),gen)->Delete();
		}

		if (IsKeyPressed(KEY_ENTER))
		{
			size_t layerCount = network->size();
			size_t nodeCount = 0;
			size_t synapseCount = 0;
			for (Layer* layer : *network)
			{
				nodeCount += layer->size();
				for (Node* node : *layer)
				{
					synapseCount += node->outputs.size();
				}
			}
			std::cout << "layerCount: " << layerCount << std::endl;
			std::cout << "nodeCount: " << nodeCount << std::endl;
			std::cout << "synapseCount: " << synapseCount << std::endl;
		}

		if (IsKeyPressed(KEY_S))
		{
			NeuralNetwork::Save(*network, "test.bin");
		}
		if (IsKeyPressed(KEY_L))
		{
			delete network;
			network = NeuralNetwork::Load(functions, "test.bin");
			netVis.network = network;
		}
		if (IsKeyPressed(KEY_C))
		{
			coppiedNetwork = NeuralNetwork::Copy(network);

		}
		if (IsKeyPressed(KEY_P))
		{
			if (coppiedNetwork)
			{
				delete network;
				network = coppiedNetwork;
				coppiedNetwork = nullptr;
				netVis.network = network;
			}
		}
		if (IsKeyPressed(KEY_SPACE))
		{
			running = !running;
		}

		BeginDrawing();
		ClearBackground(BLACK);
		netVis.Draw();
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		EndDrawing();
	}
}
