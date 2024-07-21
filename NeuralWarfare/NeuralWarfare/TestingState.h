#pragma once
#include "GameState.h"
#include <future>
#include "NeuralWarfareTrainers.h"
#include "ActivationFunctions.h"


class TrainerListEntry;

/// <summary>
/// Gamestate for testing AI
/// </summary>
class TestingState : public GameState
{
public:
	/// <summary>
	/// TestingState constructor
	/// </summary>
	/// <param name="app">Application reference allows the gamestate to change the state or access configs </param>
	TestingState(Application& app);
	virtual ~TestingState();

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

private:
	UISliderContainer* trainerlList;

	float modelListEntryYPos,
		modelListEntryWidth,
		modelListEntryHeight,
		modelListEntryHSpaceing,
		modelListEntryInputTextSize,
		modelListEntryXPos;

	AddFunction addfunction;
	TanhFunction tanhFunction;
	SigmoidFunction sigmoidFunction;
	std::vector<ActivationFunction*> functions;

	NeuralWarfareEngine eng;
	Rectangle engDrawRec;

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<Trainer*> trainers;

	TrainerListEntry* bestTrainerListEntry;

	float resetTimer = 0;
	std::future<void>* trainerFuture = nullptr;
	size_t stepsPerFrame = 1;

	/// <summary>
	/// Function to load a model from file 
	/// </summary>
	/// <param name="modelName"></param>
	/// <param name="totalTrainers"></param>
	void LoadTrainer(std::string modelName, size_t totalTrainers);


	/// <summary>
	/// Function to find the best preforming trainer for the best model display
	/// </summary>
	void FindBestTrainer();
};