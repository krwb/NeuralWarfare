#include "TrainingState.h"
#include "Application.h"

TrainingState::TrainingState(Application& app) : GameState(app), eng(app.gen, { app.config.engine.sizeX,app.config.engine.sizeY }), engDrawRec({}), netVis(nullptr, {})
{
	netVis.drawRec = {
	app.config.app.screenWidth * 0.6f,
	app.config.app.screenHeight * 0.02f,
	app.config.app.screenWidth * 0.2f,
	app.config.app.screenHeight * 0.25f
	};

    engDrawRec = { 
		app.config.app.screenWidth * 0.28f, 
		app.config.app.screenHeight * 0.28f,
        app.config.app.screenWidth * 0.70f, 
        app.config.app.screenHeight * 0.70f 
	};

	functions.push_back(&addfunction);
	functions.push_back(&sigmoidFunction);
	functions.push_back(&tanhFunction);

	ui = new UIContainer(app.config.ui.primaryColor, app.config.ui.secondaryColor, app.config.ui.textColor);

    new UILabeledButton<UIFunctionButton<UIButtonRec>>{
        ui, "BACK", app.config.app.screenHeight * 0.05f,
        [&app]() { app.ChangeState(EgameState::MAINMENU); },
        Rectangle{app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.01f, app.config.app.screenWidth * 0.1f, app.config.app.screenHeight * 0.05f}
    };
	
	// speed control buttons
	
	// Button size
	float buttonWidth = app.config.app.screenWidth * 0.05f;
	float buttonHeight = buttonWidth;

	// Padding
	float paddingX = (app.config.app.screenWidth * 0.25 - (4 * buttonWidth)) / 5.0f;
	float paddingY = (app.config.app.screenHeight * 0.25 - (2 * buttonHeight)) / 3.0f;

	size_t* stepsPerFramePointer = &stepsPerFrame;
	// Create 8 buttons
	for (int i = 0; i < 8; ++i) {
		float x = paddingX + (i % 4) * (buttonWidth + paddingX);
		float y = paddingY + (i / 4) * (buttonHeight + paddingY) + app.config.app.screenHeight * 0.75f;
		size_t buttonSpeedMult = (size_t)std::round(std::pow(i + 1, 2) / 2);
		new UILabeledButton<UIFunctionButton<UIButtonRec>>{
			ui, std::to_string(buttonSpeedMult) +"x", buttonHeight / 2,
			[stepsPerFramePointer, buttonSpeedMult]() { *stepsPerFramePointer = buttonSpeedMult; },
			Rectangle{x, y, buttonWidth, buttonHeight}
		};
	}
	
	trainerList = new UISliderContainer(ui, { app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.15f, app.config.app.screenWidth * 0.025f, app.config.app.screenHeight * 0.6f },1);
}

TrainingState::~TrainingState()
{
	delete ui;
	if (trainerFuture)
	{
		trainerFuture->wait();
		delete trainerFuture;
	}
	while (!trainers.empty())
	{
		delete trainers.back();
		trainers.pop_back();
	}
	while (!envs.empty())
	{
		delete envs.back();
		envs.pop_back();
	}
}


void TrainingState::Load()
{
}

void TrainingState::Unload()
{
}

void TrainingState::Update(float deltaTime)
{
    ui->update();

	for (size_t i = 0; i < stepsPerFrame; i++)
	{
		resetTimer += 1.0f / 60.0f;//deltaTime;

		if (resetTimer > 5)
		{
			eng.Reset();
			resetTimer = 0;
		}
		for (Trainer* trainer : trainers)
		{
			trainer->ObserveEnvironment();
		}

		delete trainerFuture;
		trainerFuture = new std::future<void>(std::async(std::launch::async, UpdateTrainers, std::ref(trainers)));
		eng.Update(4);
		trainerFuture->wait();

		for (Trainer* trainer : trainers)
		{
			trainer->ExecuteAction();
		}
	}
	if (IsKeyPressed(KEY_SPACE))
	{
		AddNewModel();
	}
}

void TrainingState::Draw()
{
    ui->draw();
	eng.Draw(engDrawRec);
    DrawRectangleLinesEx(engDrawRec, 5, app.config.ui.secondaryColor);
	DrawRectangleRec(netVis.drawRec, app.config.ui.secondaryColor );
}

void TrainingState::AddNewModel()
{
	size_t inputSize = NeuralWarfareEnv::MyObservation(eng, nullptr).NNInputSize();
	size_t outputSize = NeuralWarfareEnv::MyAction(0).NNOutputSize();
	NeuralNetwork* network = new NeuralNetwork(functions);
	for (size_t i = 0; i < inputSize; i++)
	{
		network->AddInput(new Node(nullptr, &addfunction));
	}
	for (size_t i = 0; i < (inputSize + outputSize) / 2; i++)
	{
		new Node(*std::prev(network->end()), &tanhFunction);
	}
	new Layer(network);
	for (size_t i = 0; i < outputSize; i++)
	{
		network->AddOutput(new Node(nullptr, &sigmoidFunction));
	}
	GeneticAlgorithmNNTrainer::MyHyperparameters hyperperameters("hyperperameters.xml");

	envs.push_back(new NeuralWarfareEnv(eng, eng.AddTeam(200, 2, {0,0})));
	GeneticAlgorithmNNTrainer* trainer = new GeneticAlgorithmNNTrainer(envs.back(), app.gen, hyperperameters, network);
	trainers.push_back(trainer);

	Vec2 newEntryPos;
	trainerList->value = 0;
	trainerList->griped = false;
	trainerList->update();
	if (trainerList->childElements.empty())
	{
		newEntryPos = { app.config.app.screenWidth * 0.05f, app.config.app.screenHeight * 0.15f };
	}
	else
	{
		newEntryPos = { app.config.app.screenWidth * 0.05f, trainerList->childElements.back()->getPos().y + app.config.app.screenWidth * 0.05f };
	}
	new TrainerListEntry(trainerList, { newEntryPos.x,newEntryPos.y ,app.config.app.screenWidth * 0.1f ,app.config.app.screenWidth * 0.05f }, trainer);
}

TrainerListEntry::TrainerListEntry(UIContainer* parentContainer, Rectangle rec, Trainer* trainer) : UIElement(parentContainer), UISubContainer(parentContainer), UIRectangle(parentContainer, rec), trainer(trainer)
{
}

TrainerListEntry::~TrainerListEntry()
{
}

void TrainerListEntry::draw(float alpha)
{
	UIRectangle::draw(alpha);
	UISubContainer::draw(alpha);
}

void TrainerListEntry::update()
{
	UISubContainer::update();
}

void TrainerListEntry::setPos(const Vec2& pos)
{
	Vec2 move = getPos() - pos;
	UIRectangle::setPos(pos);
	for (UIElement* e : childElements)
	{
		e->setPos(getPos() + move);
	}
}
