#pragma once
#include "GameState.h"
#include "RaylibGUI.h"
class MenuState : public GameState
{
public:
	MenuState(Application& app);
	virtual ~MenuState();

	virtual void Load();
	virtual void Unload();

	virtual void Update(float deltaTime);
	virtual void Draw();

protected:
private:
	UIContainer* ui;
};