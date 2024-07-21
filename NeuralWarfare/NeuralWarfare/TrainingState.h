#pragma once
#include "GameState.h"
#include "NeuralWarfareTrainers.h"
#include <future>
#include "ActivationFunctions.h"
#include "RaylibNetworkVis.h"

class TrainingState;
class TrainerListEntry : public UISubContainer, public UIRectangle
{
public:
	TrainerListEntry(UIContainer* parentContainer, Rectangle rec, Trainer* trainer, std::string modelName);
	virtual ~TrainerListEntry();

	virtual void draw(float alpha = 0);

	virtual void update();

	virtual void setPos(const Vec2& pos);
	
	virtual Vec2 getPos() { return UIRectangle::getPos(); }

	Trainer* trainer;
	UITextLine* nameText;
private:
};


class UIPopup : public UISubContainer, public UIRectangle
{
public:
	UIPopup(UIContainer* parentContainer, Rectangle rec) : UIElement(parentContainer), UISubContainer(parentContainer), UIRectangle(parentContainer, rec) {};
	virtual ~UIPopup() {};
	virtual void draw(float alpha = 0)
	{
		UIRectangle::draw(alpha - 100);
		UISubContainer::draw(alpha);
	}
	virtual void update()
	{
		UIRectangle::update();
		UISubContainer::update();
	}

	virtual void setPos(const Vec2& pos) { UIRectangle::setPos(pos); UISubContainer::setPos(pos); };

	virtual Vec2 getPos() { return UIRectangle::getPos(); }

private:

};


class TrainingState : public GameState
{
public:
	TrainingState(Application& app);
	virtual ~TrainingState();

	virtual void Load();
	virtual void Unload();

	virtual void Update(float deltaTime);
	virtual void Draw();

	void SetSelectedTrainer(TrainerListEntry* trainer);
	void AddNewModel();
	void LoadModel(std::string modelName);
	void SetLoadPopupHidden(bool hidden);
	void AddTrainer(NeuralNetwork* network, std::string modelName);

	void SaveSelectedModel();

	TrainerListEntry* selectedTrainer = nullptr;
protected:
private:
	UISliderContainer* trainerList;
	UISliderContainer* hyperparameterControls;

	UITextInput<UIBackgroundlessTextBox>
		* topAgentCountInput,
		* mutationCountInput,
		* biasMutationRateInput,
		* biasMutationMagnitudeInput,
		* weightMutationRateInput,
		* weightMutationMagnitudeInput,
		* synapseMutationRateInput,
		* newSynapseMagnitudeInput,
		* nodeMutationRateInput,
		* layerMutationRateInput,
		* newLayerSizeAverageInput,
		* newLayerSizeRangeInput;

	UIPopup* loadModelInput;
	UITextInput<UIBackgroundlessTextBox>* loadNameInput;
	UITextInput<UIBackgroundlessTextBox>* nameInput;

	NeuralWarfareEngine eng;
	Rectangle engDrawRec;
	NetworkVis netVis;

	float resetTimer = 0;
	std::future<void>* trainerFuture = nullptr;
	size_t stepsPerFrame = 1;

	AddFunction addfunction;
	TanhFunction tanhFunction;
	SigmoidFunction sigmoidFunction;
	std::vector<ActivationFunction*> functions;

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<Trainer*> trainers;

	void UpdateHyperparameterControls();
};