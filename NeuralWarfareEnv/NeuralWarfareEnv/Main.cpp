#include <iostream>
#include "NeuralWarfareEnv.h"
int main()
{
    NeuralWarfareEngine eng;
    NeuralWarfareEnv testEnv(eng,1);
    std::vector<Environment::StepResult> srs = testEnv.GetResult();

    InitWindow(600, 400, "test");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }
}