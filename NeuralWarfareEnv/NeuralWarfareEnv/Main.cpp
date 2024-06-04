#include <iostream>
#include "NeuralWarfareEnv.h"
#include <random>
#include <chrono>
#include <string>
#include "KDTree.h"

int main()
{

    std::mt19937 gen(12345);
    std::uniform_int_distribution<size_t> dis(1, 4);

    NeuralWarfareEngine eng(Vec2(1200, 800));
    std::list<NeuralWarfareEnv> envs;
    Rectangle drawRec{ 50, 50, 1100, 700 };
    for (size_t i = 0; i < 2; i++)
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
            std::vector<size_t> actions;
            for (size_t i = 0; i < 200; i++)
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
            std::vector<Vec2> points;
            for (NeuralWarfareEngine::Agent agent : eng.agents)
            {
                points.push_back(agent.pos);
            }
            KDTree kdtree(points);
            
            std::vector<Vec2> drawPoints;
            for (NeuralWarfareEngine::Agent& agent : eng.agents)
            {
                std::vector<Vec2> n = kdtree.FindNearestNeighbors(agent.pos, 10);
                drawPoints.insert(drawPoints.end(), n.begin(), n.end());
            }

            if (false)
            {
                Vec2 drawScale = Vec2(drawRec.width, drawRec.height) / eng.simSize / 2;
                Vec2 drawCenter(drawRec.x + drawRec.width / 2, drawRec.y + drawRec.height / 2);
                Vec2 query = Vec2(GetMousePosition()) / drawScale - drawCenter / drawScale;
                for (Vec2 point : drawPoints)
                {
                    DrawCircle(drawCenter.x + point.x * drawScale.x, drawCenter.y + point.y * drawScale.y, 2, WHITE);
                }
            }
        }
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            std::vector<Vec2> drawPoints;
            for (NeuralWarfareEngine::Agent& agent1 : eng.agents)
            {
                size_t count = 10;
                KDTree::MaxHeap maxHeap;
                for (NeuralWarfareEngine::Agent& agent2 : eng.agents)
                {

                    float distance = (agent1.pos - agent2.pos).Length();
                    if (maxHeap.size() < count)
                    {
                        maxHeap.emplace(distance, agent2.pos);
                    }
                    else if (distance < maxHeap.top().first)
                    {
                        maxHeap.pop();
                        maxHeap.emplace(distance, agent2.pos);
                    }
                }
            }
            if (false)
            {
                Vec2 drawScale = Vec2(drawRec.width, drawRec.height) / eng.simSize / 2;
                Vec2 drawCenter(drawRec.x + drawRec.width / 2, drawRec.y + drawRec.height / 2);
                Vec2 query = Vec2(GetMousePosition()) / drawScale - drawCenter / drawScale;
                for (Vec2 point : drawPoints)
                {
                    DrawCircle(drawCenter.x + point.x * drawScale.x, drawCenter.y + point.y * drawScale.y, 2, WHITE);
                }
            }
        }

        EndDrawing();
    }
}