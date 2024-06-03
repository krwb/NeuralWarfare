#include <iostream>
#include "NeuralWarfareEnv.h"
#include <random>
#include <chrono>
#include <string>

int main()
{
    NeuralWarfareEngine eng(Vec2(1200, 800));
    std::list<NeuralWarfareEnv> envs;
    for (size_t i = 0; i < 20; i++)
    {
        envs.push_back(NeuralWarfareEnv(eng, eng.AddTeam(200)));
    }

    std::mt19937 gen(12345);
    std::uniform_int_distribution<size_t> dis(1, 4);
    std::vector<size_t> actions;
    for (size_t i = 0; i < 200; i++)
    {
        actions.push_back(dis(gen));
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

            env.TakeAction(actions);
        }
        eng.Update(2);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(("FPS: " + std::to_string(1 / deltaTime)).c_str(), 0, 0, 18, WHITE);

        eng.Draw(Rectangle{ 50, 50, 1100, 700 });
        EndDrawing();
    }
}