#include "MainMenuState.h"
#include "raylib.h"
#include "Application.h"
#include "iostream"

MenuState::MenuState(Application& app) : GameState(app)
{
	ui = new UIContainer(app.config.ui.primaryColor, app.config.ui.secondaryColor, app.config.ui.textColor);

    float buttonHeight = app.config.app.screenHeight * 0.05;
    float buttonWidth = app.config.app.screenWidth - app.config.app.screenWidth * 0.12f;
    float buttonVSpaceing = buttonHeight * 1.5;
    float buttonXpos = app.config.app.screenWidth * 0.06f;
    float buttonYpos = app.config.app.screenHeight * 0.4f;

    new UITextLine(ui, { app.config.app.screenWidth * 0.5f,app.config.app.screenHeight * 0.2f }, "Neural Warfare", app.config.app.screenHeight * 0.1f);

    new UILabeledButton<UIFunctionButton<UIButtonRec>>{
        ui, "TRAIN", buttonHeight,
        [&app]() { app.ChangeState(EgameState::TRAINING); },
        Rectangle{buttonXpos, buttonYpos, buttonWidth, buttonHeight}
    };

    new UILabeledButton<UIFunctionButton<UIButtonRec>>{
    ui, "TEST", buttonHeight,
    [&app]() { app.ChangeState(EgameState::TESTSELECTION); },
    Rectangle{buttonXpos, buttonYpos + buttonVSpaceing, buttonWidth, buttonHeight}
    };

    new UILabeledButton<UIFunctionButton<UIButtonRec>>{
        ui, "QUIT", buttonHeight,
        [&app]() { app.Quit(); },
        Rectangle{buttonXpos, buttonYpos + buttonVSpaceing * 2, buttonWidth, buttonHeight}
    };
}


MenuState::~MenuState()
{
	delete ui;
}

void MenuState::Load()
{

}

void MenuState::Unload()
{
}

void MenuState::Update(float deltaTime)
{
	ui->update();
}

void MenuState::Draw()
{
	ui->draw();
}
