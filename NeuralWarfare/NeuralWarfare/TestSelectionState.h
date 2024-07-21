#pragma once
#include "GameState.h"
class TestSelectionState;
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

class TestSelectionState : public GameState
{
public:
	TestSelectionState(Application& app);
	virtual ~TestSelectionState();

	virtual void Load();
	virtual void Unload();

	virtual void Update(float deltaTime);
	virtual void Draw();

	virtual void AddModel(ModelListEntry* modelEntry);
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

	void RemoveDeselectedModels();

	void GetModels();

};