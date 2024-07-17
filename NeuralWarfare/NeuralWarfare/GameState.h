#pragma once
class Application;
enum class EgameState {
	NONE,
	MAINMENU,
	TRAINING
};

class GameState
{
public:
	Application& app;
	GameState(Application& app) : app(app) {};
	virtual ~GameState() {};

	virtual void Load() = 0;
	virtual void Unload() = 0;

	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;


protected:
};