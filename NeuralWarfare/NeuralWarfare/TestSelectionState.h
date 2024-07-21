#pragma once
#include "GameState.h"
class TestSelectionState;
/// <summary>
/// UIElement to store and display information on a model
/// </summary>
class ModelListEntry : public UISubContainer, public UIRectangle
{
public:
	ModelListEntry(UIContainer* parentContainer, Rectangle rec, std::string modelName, std::string details);
	ModelListEntry(UIContainer* parentContainer, Rectangle rec, ModelListEntry& listEntry);
	virtual ~ModelListEntry();

	virtual void draw(float alpha = 0);

	virtual void update();

	virtual void setPos(const Vec2& pos);

	virtual Vec2 getPos() { return UIRectangle::getPos(); }

	UITextLine* nameText;
private:
	TestSelectionState* testSelectionState;
	UITextBox* detailsText;
};

/// <summary>
/// Gamestate for selecting AI to test
/// </summary>
class TestSelectionState : public GameState
{
public:
	/// <summary>
	/// TestSelectionState constructor
	/// </summary>
	/// <param name="app">Application reference allows the gamestate to change the state or access configs </param>
	TestSelectionState(Application& app);
	virtual ~TestSelectionState();

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

	/// <summary>
	/// Function to add a model to the selected models
	/// </summary>
	/// <param name="modelEntry"> model entry from the unloaded models to be loaded</param>
	virtual void AddModel(ModelListEntry* modelEntry);

	/// <summary>
	/// Function to remove a model from the selected models
	/// </summary>
	/// <param name="modelEntry"> model entry from the loaded models to be removed</param>
	virtual void RemoveModel(ModelListEntry* modelEntry);

	UISliderContainer* selectedModelList;

private:
	UISliderContainer* modelList;

	float modelListEntryYPos,
		modelListEntryWidth,
		modelListEntryHeight,
		modelListEntryVSpaceing,
		modelListEntryInputTextSize,
		modelListEntryXPos,
		selectedModelListEntryXPos;


	std::vector<ModelListEntry*> modelsForRemoval;

	/// <summary>
	/// Function to remove models that have been marked for removal by RemoveModel
	/// </summary>
	void RemoveDeselectedModels();

	/// <summary>
	/// function to load information about all available models
	/// </summary>
	void GetModels();

};