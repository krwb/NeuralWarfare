#pragma once
#include "GameState.h"
#include "NeuralWarfareTrainers.h"
#include <future>
#include "ActivationFunctions.h"
#include "RaylibNetworkVis.h"

class TrainingState;
/// <summary>
/// UIElement for displaying and storing information on a trainer
/// </summary>
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

/// <summary>
/// UIPopup class basically just a UISubContainer with a built in UIRectangle
/// </summary>
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

/// <summary>
/// Gamestate for training AI
/// </summary>
class TrainingState : public GameState
{
public:
	/// <summary>
	/// TrainingState constructor
	/// </summary>
	/// <param name="app">Application reference allows the gamestate to change the state or access configs </param>
	TrainingState(Application& app);
	virtual ~TrainingState();

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
	/// Function to select the trainer linked to the training controls
	/// </summary>
	/// <param name="trainer"></param>
	void SetSelectedTrainer(TrainerListEntry* trainer);

	/// <summary>
	/// Creates a new model
	/// </summary>
	void AddNewModel();

	/// <summary>
	/// loads an existing model from the model folder
	/// </summary>
	/// <param name="modelName"> name of the model to be loaded</param>
	void LoadModel(std::string modelName);

	/// <summary>
	/// sets the load PopUp as hidden or visible
	/// </summary>
	/// <param name="hidden"> true if PopUp should be hidden</param>
	void SetLoadPopupHidden(bool hidden);

	/// <summary>
	/// creates a trainer for the a model and adds it for training
	/// </summary>
	/// <param name="network"></param>
	/// <param name="modelName"></param>
	void AddTrainer(NeuralNetwork* network, std::string modelName);

	/// <summary>
	/// saves the currently selected model
	/// </summary>
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

	/// <summary>
	/// Update the Hyperparameter Controls
	/// </summary>
	void UpdateHyperparameterControls();
};