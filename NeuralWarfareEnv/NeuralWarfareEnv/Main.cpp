#include <iostream>
#include "NeuralWarfareEnv.h"
#include <random>
#include <chrono>
#include <string>
#include "KDTree.h"


std::vector<NeuralWarfareEngine::Agent*> getNearestNeighbors(NeuralWarfareEngine& eng, Vec2 query, size_t count)
{
	KDTree::MaxHeap maxHeap;
	for (NeuralWarfareEngine::Agent& agent : eng.agents)
	{
		float distance = (query - agent.pos).Length();
		if (maxHeap.size() < count)
		{
			maxHeap.emplace(distance, &agent);
		}
		else if (distance < maxHeap.top().first)
		{
			maxHeap.pop();
			maxHeap.emplace(distance, &agent);
		}
	}
	std::vector<NeuralWarfareEngine::Agent*> neighbors;
	while (!maxHeap.empty()) {
		neighbors.push_back(maxHeap.top().second);
		maxHeap.pop();
	}
	return neighbors;
}


int main()
{

	std::mt19937 gen(12345);
	std::uniform_int_distribution<size_t> dis(1, 4);

	NeuralWarfareEngine eng(Vec2(1200, 800));
	std::list<NeuralWarfareEnv> envs;
	Rectangle drawRec{ 50, 50, 1100, 700 };
	size_t teamSize = 200;
	for (size_t i = 0; i < 20; i++)
	{
		envs.push_back(NeuralWarfareEnv(eng, eng.AddTeam(teamSize)));
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
			std::vector<size_t> actions;
			for (size_t i = 0; i < teamSize; i++)
			{
				actions.push_back(dis(gen));
			}
			env.TakeAction(actions);
		}
		eng.Update(5);
		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);
		eng.Draw(drawRec);
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			for (NeuralWarfareEngine::Agent& agent : eng.agents)
			{
			    std::vector<NeuralWarfareEngine::Agent*> n = eng.kdTree->FindNearestNeighbors(agent.pos, 10,
					[agent](const NeuralWarfareEngine::Agent& a)
					{
						return a.teamId != agent.teamId;
					}
				
				);
			}

			//Vec2 drawScale = Vec2(drawRec.width, drawRec.height) / eng.simSize / 2;
			//Vec2 drawCenter(drawRec.x + drawRec.width / 2, drawRec.y + drawRec.height / 2);
			//Vec2 query = Vec2(GetMousePosition()) / drawScale - drawCenter / drawScale;
			//std::vector<NeuralWarfareEngine::Agent*> drawPoints = kdtree.FindNearestNeighbors(query, 10);
			//for (NeuralWarfareEngine::Agent* agent : drawPoints)
			//{
			//	Vec2 point = agent->pos;
			//	DrawCircle(drawCenter.x + point.x * drawScale.x, drawCenter.y + point.y * drawScale.y, 5, ORANGE);
			//}
		}

		if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		{
			for (NeuralWarfareEngine::Agent& agent : eng.agents)
			{
				std::vector<NeuralWarfareEngine::Agent*> n = getNearestNeighbors(eng,agent.pos, 20);
			}

			//Vec2 drawScale = Vec2(drawRec.width, drawRec.height) / eng.simSize / 2;
			//Vec2 drawCenter(drawRec.x + drawRec.width / 2, drawRec.y + drawRec.height / 2);
			//Vec2 query = Vec2(GetMousePosition()) / drawScale - drawCenter / drawScale;
			//std::vector<NeuralWarfareEngine::Agent*> drawPoints = getNearestNeighbors(eng,query, 10);
			//for (NeuralWarfareEngine::Agent* agent : drawPoints)
			//{
			//	Vec2 point = agent->pos;
			//	DrawCircle(drawCenter.x + point.x * drawScale.x, drawCenter.y + point.y * drawScale.y, 5, ORANGE);
			//}
		}

		EndDrawing();
	}
}