#include <iostream>
#include "NeuralWarfareEnv.h"
#include <chrono>
#include <string>
#include <future>

std::vector<std::list<Environment::Action>*> GetActions(std::vector<std::list<Environment::StepResult>*> srts)
{
	std::vector <std::list<Environment::Action>*> allActions;

	for (size_t i = 0; i < srts.size(); i++)
	{
		std::list<Environment::Action>* actions = new std::list<Environment::Action>();
		for (Environment::StepResult& sr : *srts[i])
		{
			double nearestHostileDirection = sr.observation->GetForTest();
			size_t action = nearestHostileDirection == 0 ? 0 : nearestHostileDirection < 0 ? 1 : 2;
			actions->push_back({ sr, action });
		}
		allActions.push_back(actions);
	}
	return allActions;
}

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> dis(1, 2);
	NeuralWarfareEngine eng(gen, Vec2(550, 350));

	std::vector<NeuralWarfareEnv> envs;

	std::uniform_real_distribution<float> spawnXDis(-eng.simSize.x, eng.simSize.x);
	std::uniform_real_distribution<float> spawnYDis(-eng.simSize.y, eng.simSize.y);
	for (size_t i = 0; i < 10; i++)
	{
		envs.push_back(NeuralWarfareEnv(eng, eng.AddTeam(100, 2, { spawnXDis(gen),spawnYDis(gen) })));
	}

	Rectangle drawRec{ 50, 50, 1100, 700 };

	InitWindow(1200, 800, "test");
	SetTargetFPS(60);

	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();

	float resetTimer = 0;

	std::future<std::vector<std::list<Environment::Action>*>>* actionFuture = nullptr;

	bool running = true;

	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;

		if (running)
		{
			resetTimer += deltaTime;
		}

		if (resetTimer > 20)
		{
			eng.Reset();
			resetTimer = 0;
		}

		if (IsKeyPressed(KEY_R))
		{
			eng.Reset();
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			running = !running;
		}

		if (running || IsKeyPressed(KEY_PERIOD))
		{
			for (size_t i = 0; i < 5; i++)
			{
				//if (actionFuture)
				//{
				//	std::vector <std::list<Environment::Action>*> actions = actionFuture->get();
				//	delete actionFuture;
				//	for (size_t i = 0; i < envs.size(); i++)
				//	{
				//		envs[i].TakeAction(*actions[i]);
				//	}
				//}

				std::vector<std::list<Environment::StepResult>*> srts;
				for (size_t i = 0; i < envs.size(); i++)
				{
					srts.push_back(envs[i].GetResult());
				}
				//actionFuture = new std::future<std::vector<std::list<Environment::Action>*>>(std::async(std::launch::async, GetActions, srts));
				
				std::vector <std::list<Environment::Action>*> actions = GetActions(srts);
				for (size_t i = 0; i < envs.size(); i++)
				{
					envs[i].TakeAction(*actions[i]);
				}

				eng.Update(1);
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		eng.Draw(drawRec);
		EndDrawing();
	}
}