#pragma once
#include "GameState.h"
#include <future>
#include "NeuralWarfareTrainers.h"
#include "ActivationFunctions.h"


class TrainerListEntry;
class TestingState : public GameState
{
public:
	TestingState(Application& app);
	virtual ~TestingState();

	virtual void Load();
	virtual void Unload();

	virtual void Update(float deltaTime);
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

	void LoadTrainer(std::string modelName, size_t totalTrainers);

	void FindBestTrainer();
};