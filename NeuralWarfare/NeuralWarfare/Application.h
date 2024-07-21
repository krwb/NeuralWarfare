#pragma once
#include "GameState.h"
#include "raylib.h"
#include "Configs.h"
#include <random>
class Application
{
public:

	Application(Config& config ) : config(config), gen(std::random_device()()){};
	~Application() {};

	void Run();
	void ChangeState(EgameState newstate);
	void Quit() { shouldExit = true; }

	Config& config;
	std::mt19937 gen;
	GameState* currentGameState = nullptr;
protected:
private:
	void ChangeToNextState();
	bool shouldExit = false;
	EgameState nextState = EgameState::NONE;
};