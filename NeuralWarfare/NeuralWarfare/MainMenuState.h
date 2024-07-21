#pragma once
#include "GameState.h"

/// <summary>
/// Gamestate for selecting gamestates
/// </summary>
class MenuState : public GameState
{
public:

	/// <summary>
	/// MenuState constructor
	/// </summary>
	/// <param name="app">Application reference allows the gamestate to change the state or access configs </param>
	MenuState(Application& app);
	virtual ~MenuState();

	/// <summary>
	/// Function to load the gamestate (called after previous gamestate has been deleted
	/// </summary>
	virtual void Load();

	/// <summary>
	/// Function to Unload the gamestate (called before previous gamestate has been created
	/// </summary>
	virtual void Unload();

	/// <summary>
	/// Primary update function
	/// </summary>
	/// <param name="deltaTime"> frame delta</param>
	virtual void Update(float deltaTime);

	/// <summary>
	/// Primary draw function
	/// </summary>
	virtual void Draw();

protected:
private:
};