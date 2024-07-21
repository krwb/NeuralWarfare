#include "Application.h"
#include "MainMenuState.h"
#include <chrono>
#include "TrainingState.h"
#include "TestSelectionState.h";
#include "TestingState.h"
void Application::Run()
{
    InitWindow(config.app.screenWidth, config.app.screenHeight, "NeuralWarfare");
    SetTargetFPS(config.app.targetFPS);
    auto framestart = std::chrono::high_resolution_clock::now();
    auto frameend = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(frameend - framestart).count();

    ChangeState(EgameState::MAINMENU);
    while (!WindowShouldClose() && !shouldExit)
    {
        ChangeToNextState();
        frameend = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(frameend - framestart).count();
        framestart = frameend;

        currentGameState->Update(deltaTime);
        BeginDrawing();
        ClearBackground(config.ui.backgroundColor);
        currentGameState->Draw();
        std::string fpsString = "FPS: " + std::to_string(1 / deltaTime);
        DrawText(fpsString.c_str(), config.app.screenWidth - MeasureText(fpsString.c_str(),18), 0, 18, WHITE);
        EndDrawing();

    }
}

void Application::ChangeState(EgameState newState)
{
    nextState = newState;
}

void Application::ChangeToNextState()
{
    if (nextState == EgameState::NONE) return;
    if (currentGameState)
    {
        currentGameState->Unload();
    }
    GameState* newGameState;
    switch (nextState)
    {
    case EgameState::MAINMENU:      newGameState = new MenuState(*this);            break;
    case EgameState::TRAINING:      newGameState = new TrainingState(*this);        break;
    case EgameState::TESTSELECTION: newGameState = new TestSelectionState(*this);   break;
    case EgameState::TESTING: newGameState = new TestingState(*this);               break;
    default:    // Handle invalid gamestate
        std::cout << "Error: InvalidGamestate\n";
        newGameState = new MenuState(*this);
        break;
    }
    if (currentGameState)
    {
        delete currentGameState;
    }
    newGameState->Load();
    currentGameState = newGameState;
    nextState = EgameState::NONE;
}
