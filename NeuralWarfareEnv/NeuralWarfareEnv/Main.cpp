#include <iostream>
#include "NeuralWarfareEnv.h"
#include <chrono>
#include <string>




int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dis(1, 2);
	NeuralWarfareEngine eng(gen, Vec2(1100, 700));
	std::list<NeuralWarfareEnv> envs;
	Rectangle drawRec{ 50, 50, 1100, 700 };
	for (size_t i = 0; i < 10; i++)
	{
		envs.push_back(NeuralWarfareEnv(eng, eng.AddTeam(200)));
	}


	InitWindow(1200, 800, "test");
	SetTargetFPS(120);
	auto framestart = std::chrono::high_resolution_clock::now();
	auto frameend = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(frameend - framestart).count();
	while (!WindowShouldClose())
	{
		frameend = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(frameend - framestart).count();
		framestart = frameend;

		for (auto& env : envs)
		{
			std::vector<Environment::StepResult*> sr = env.GetResult();
			std::vector<size_t> actions;
			while (!sr.empty())
			{
				delete sr.back();
				sr.pop_back();
				actions.push_back(dis(gen));
			}
			env.TakeAction(actions);
		}
		BeginDrawing();
		eng.Update(1);
		ClearBackground(BLACK);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		eng.Draw(drawRec);
		EndDrawing();
	}
}