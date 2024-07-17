#pragma once
#include "GameState.h"
#include "RaylibGUI.h"
#include "NeuralWarfareTrainers.h"
#include <future>
#include "ActivationFunctions.h"
#include "RaylibNetworkVis.h"

class TrainerListEntry : public UISubContainer, public UIRectangle
{
public:
	TrainerListEntry(UIContainer* parentContainer, Rectangle rec, Trainer* trainer);
	~TrainerListEntry();

	virtual void draw(float alpha = 0);


	virtual void update();

	virtual void setPos(const Vec2& pos);

private:
	Trainer* trainer;
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

protected:
private:
	UIContainer* ui;
	UISliderContainer* trainerList;
	NeuralWarfareEngine eng;
	Rectangle engDrawRec;
	NetworkVis netVis;

	std::future<void>* trainerFuture = nullptr;
	float resetTimer = 0;
	size_t stepsPerFrame = 1;

	AddFunction addfunction;
	TanhFunction tanhFunction;
	SigmoidFunction sigmoidFunction;
	std::vector<ActivationFunction*> functions;

	std::vector<NeuralWarfareEnv*> envs;
	std::vector<Trainer*> trainers;

	static void UpdateTrainers(std::vector<Trainer*>& trainers)
	{
		for (Trainer* trainer : trainers)
		{
			trainer->Update();
		}
	}

	void AddNewModel();


};