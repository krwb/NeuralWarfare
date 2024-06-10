#include <iostream>
#include "NeuralWarfareEnv.h"
#include <chrono>
#include <string>

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<size_t> dis(1, 2);
	NeuralWarfareEngine eng(gen, Vec2(550, 350));	
	
	std::vector<NeuralWarfareEnv> envs;

	std::uniform_real_distribution<float> spawnXDis(-eng.simSize.x, eng.simSize.x);
	std::uniform_real_distribution<float> spawnYDis(-eng.simSize.y, eng.simSize.y);
	for (size_t i = 0; i < 5; i++)
	{
		envs.push_back(NeuralWarfareEnv(eng, eng.AddTeam(200, 100, { spawnXDis(gen),spawnYDis(gen) })));
	}

	Rectangle drawRec{ 50, 50, 1100, 700 };

	InitWindow(1200, 800, "test");
	SetTargetFPS(120);

	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();

	float resetTimer = 0;

	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;

		//resetTimer += deltaTime;

		//if (resetTimer > 5)
		//{
		//	eng.Reset();
		//	resetTimer = 0;
		//}

		for (size_t i = 0; i < 5; i++)
		{
			for (size_t i = 0; i < envs.size(); i++)
			{
				std::vector<Environment::StepResult*> sr = envs[i].GetResult();

				std::vector<Environment::Action> actions;
				while (!sr.empty())
				{
					std::vector<double> d = sr.back()->observation->GetForNN();
					double nearestHostileDirection = 0;
					if (!d.empty())
					{
						nearestHostileDirection = d.front();
					}
					size_t action = nearestHostileDirection == 0 ? 0 : nearestHostileDirection < 0 ? 1 : 2;
					actions.push_back(Environment::Action(sr.back(), action));
					delete sr.back();
					sr.pop_back();
				}

				envs[i].TakeAction(actions);
			}
			eng.Update(2);
		}

		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		eng.Draw(drawRec);
		EndDrawing();
	}
}