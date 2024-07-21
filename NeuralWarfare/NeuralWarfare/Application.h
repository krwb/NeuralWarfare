#pragma once
#include "GameState.h"
#include "raylib.h"
#include "Configs.h"
#include <random>

/// <summary>
/// Application class to handle gamestates and the main loop 
/// </summary>
class Application
{
public:

	/// <summary>
	/// Application constructor
	/// </summary>
	/// <param name="config">Config class that has been loaded with the session configs</param>
	Application(Config& config ) : config(config), gen(std::random_device()()){};
	~Application() {};

	/// <summary>
	/// Runs the main loop
	/// </summary>
	void Run();

	/// <summary>
	/// Function to allow gamestates to tell the Application to change the game state
	/// </summary>
	/// <param name="newstate"></param>
	void ChangeState(EgameState newstate);

	/// <summary>
	/// Function to allow the gamestates to tell the Application to exit
	/// </summary>
	void Quit() { shouldExit = true; }

	Config& config;
	std::mt19937 gen;
	GameState* currentGameState = nullptr;
protected:
private:

	/// <summary>
	/// Function to change to the next gamestate (selected by ChangeState function)
	/// </summary>
	void ChangeToNextState();
	bool shouldExit = false;
	EgameState nextState = EgameState::NONE;
};