#pragma once
#include "RaylibGUI.h"
class Application;
enum class EgameState {
	NONE,
	MAINMENU,
	TRAINING,
	TESTSELECTION,
	TESTING
};

/// <summary>
/// Abstract class for gamestates
/// </summary>
class GameState
{
public:
	Application& app;
	/// <summary>
	/// GameState constructor
	/// </summary>
	/// <param name="app">Application reference allows the gamestate to change the state or access configs </param>
	GameState(Application& app) : app(app) {};
	virtual ~GameState() {};

	/// <summary>
	/// Function to load the gamestate (called after previous gamestate has been deleted
	/// </summary>
	virtual void Load() = 0;

	/// <summary>
	/// Function to Unload the gamestate (called before previous gamestate has been created
	/// </summary>
	virtual void Unload() = 0;

	/// <summary>
	/// Primary update function
	/// </summary>
	/// <param name="deltaTime"> frame delta</param>
	virtual void Update(float deltaTime) = 0;

	/// <summary>
	/// Primary draw function
	/// </summary>
	virtual void Draw() = 0;


protected:
	UIContainer* ui;

};