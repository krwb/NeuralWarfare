#include <iostream>
#include "raylib.h"
#include "NeuralNetwork.h"
#include "RaylibNetworkVis.h"
#include <random>
#include <chrono>
#include <string>
#include "ActivationFunctions.h"

int main()
{
	float windowWidth = 1200;
	float windowHeight = 800;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real<double> dis(-1, 1);

	NeuralNetwork network;
	AddFunction addfunction;

	for (size_t i = 0; i < 5; i++)
	{
		network.AddInput(new Node(&addfunction,nullptr));
	}
	network.network.insert(std::prev(network.network.end()), Layer());
	for (size_t i = 0; i < 4; i++)
	{
		Node* newNode = new Node(&addfunction, &*std::prev(std::prev(network.network.end())));
	}
	for (size_t i = 0; i < 3; i++)
	{
		network.AddOutput(new Node(&addfunction,nullptr));
	}

	network.MakeFullyConnected();

	InitWindow(windowWidth, windowHeight, "test");
	NetworkVis netVis(&network,{ windowWidth/2,windowHeight/2, 1200, 800 });

	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();

	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;
		
		std::vector<double> inputs(5);
		for (double& input : inputs)
		{
			input = dis(gen);
		}
		network.Evaluate(inputs);
		BeginDrawing();
		ClearBackground(BLACK);
		netVis.draw();
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		EndDrawing();
	}
}
