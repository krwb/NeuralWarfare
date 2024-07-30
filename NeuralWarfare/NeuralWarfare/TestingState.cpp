#include "TestingState.h"
#include "Application.h"
#include "TestSelectionState.h"
#include "TrainingState.h"
TestingState::TestingState(Application& app) : GameState(app), eng(app.gen, { app.config.engine.sizeX,app.config.engine.sizeY })
{

	functions.push_back(&addfunction);
	functions.push_back(&sigmoidFunction);
	functions.push_back(&tanhFunction);


	ui = new UIContainer(app.config.ui.primaryColor, app.config.ui.secondaryColor, app.config.ui.textColor);
	engDrawRec = {
		app.config.app.screenWidth * 0.25f,
		app.config.app.screenHeight * 0.01f,
		app.config.app.screenWidth * 0.74f,
		app.config.app.screenHeight * 0.74f
	};

	trainerlList = new UISliderContainer(ui, { app.config.app.screenWidth * 0.25f,app.config.app.screenHeight * 0.965f,app.config.app.screenWidth * 0.74f , app.config.app.screenHeight * 0.025f },15);
	
	modelListEntryXPos = app.config.app.screenWidth * 0.25f;
	modelListEntryYPos = app.config.app.screenHeight * 0.775f;
	modelListEntryWidth = app.config.app.screenWidth * 0.35f;
	modelListEntryHeight = app.config.app.screenHeight * 0.15f;
	modelListEntryHSpaceing = modelListEntryWidth * 0.6;
	modelListEntryInputTextSize = app.config.app.screenHeight * 0.025f;
	
	
	TestSelectionState* tss = static_cast<TestSelectionState*>(app.currentGameState);
	for (UIElement* element :tss->selectedModelList->childElements)
	{
		ModelListEntry* mls = dynamic_cast<ModelListEntry*>(element);
		if (mls)
		{
			LoadTrainer(mls->nameText->GetText(), tss->selectedModelList->childElements.size());
		}
		else
		{
			std::cerr << "ERROR: Failed to convert UIElement ModelListEntry\n";
		}
	}

	Rectangle rec = { app.config.app.screenWidth * 0.01f,app.config.app.screenHeight * 0.565f, app.config.app.screenWidth * 0.23f,app.config.app.screenHeight * 0.425f };
	float textSize = rec.height * 0.05f;
	bestTrainerListEntry = new TrainerListEntry(ui, rec, trainers.front() , "");

	bestTrainerListEntry->nameText->SetSize((rec.height * 0.12));

	TrainerListEntry** bestTrainerListEntryPtr = &bestTrainerListEntry;
	new UILiveText<UITextLine>{ bestTrainerListEntry,
	[bestTrainerListEntryPtr]() { return "Total Kills - This Episode : " + std::to_string(static_cast<NeuralWarfareEnv*>((*bestTrainerListEntryPtr)->trainer->env)->GetTotalKillsThisEpisode()); },
	Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.4f }, "", textSize
	};
	new UILiveText<UITextLine>{ bestTrainerListEntry,
		[bestTrainerListEntryPtr]() { return "Highest Kills - This Episode : " + std::to_string(static_cast<NeuralWarfareEnv*>((*bestTrainerListEntryPtr)->trainer->env)->GetHighestKillsThisEpisode()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.55f }, "", textSize
	};
	new UILiveText<UITextLine>{ bestTrainerListEntry,
		[bestTrainerListEntryPtr]() { return "Total Kills - All Episodes : " + std::to_string(static_cast<NeuralWarfareEnv*>((*bestTrainerListEntryPtr)->trainer->env)->GetTotalKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.7f }, "", textSize
	};
	new UILiveText<UITextLine>{ bestTrainerListEntry,
		[bestTrainerListEntryPtr]() { return "Highest Kills - All Episodes : " + std::to_string(static_cast<NeuralWarfareEnv*>((*bestTrainerListEntryPtr)->trainer->env)->GetHighestKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.85f }, "", textSize
	};

	new UITextLine(ui, { rec.x + rec.width * 0.5f ,rec.y - rec.height * 0.08f }, "Best Model", rec.height * 0.1f);


	
	float buttonWidth = app.config.app.screenWidth * 0.23f;
	float buttonHeight = app.config.app.screenHeight * 0.05f;

	float paddingY = ((buttonHeight * 1.15));
	size_t* stepsPerFramePointer = &stepsPerFrame;

	for (size_t i = 0; i < 6; i++)
	{
		float y = paddingY * i + app.config.app.screenHeight * 0.13f;
		size_t buttonSpeedMult = (size_t)std::round(std::pow(i, 2) / 2);
		new UILabeledButton<UIFunctionButton<UIButtonRec>>{
			ui, std::to_string(buttonSpeedMult) + "x", buttonHeight * 0.75f,
			[stepsPerFramePointer, buttonSpeedMult]() { *stepsPerFramePointer = buttonSpeedMult; },
			Rectangle{app.config.app.screenWidth * 0.01f, y, buttonWidth, buttonHeight}
		};
	}
	new UITextLine(ui, { rec.x + rec.width * 0.5f ,app.config.app.screenHeight * 0.1f }, "Sim Speed", rec.height * 0.1f);




	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "BACK", app.config.app.screenHeight * 0.05f,
		[&app]() { app.ChangeState(EgameState::MAINMENU); },
		Rectangle{app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.01f, app.config.app.screenWidth * 0.1f, app.config.app.screenHeight * 0.05f}
	};
}

TestingState::~TestingState()
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

void TestingState::Load()
{
}

void TestingState::Unload()
{
}

void TestingState::Update(float deltaTime)
{
	for (NeuralWarfareEnv* env : envs)
	{
		env->UpdateKillTrackers();
	}
	FindBestTrainer();
	ui->update();
	for (size_t i = 0; i < stepsPerFrame; i++)
	{
		resetTimer += 1.0f / 60.0f;//deltaTime;

		if (resetTimer > app.config.engine.resetTime)
		{
			for (NeuralWarfareEnv* env : envs)
			{
				env->Reset();
			}
			eng.Reset();
			resetTimer = 0;
		}
		for (Trainer* trainer : trainers)
		{
			trainer->ObserveEnvironment();
		}

		delete trainerFuture;
		trainerFuture = new std::future<void>(std::async(std::launch::async, UpdateTrainers, std::ref(trainers)));
		eng.Update(app.config.engine.updateDelta);
		trainerFuture->wait();
		for (Trainer* trainer : trainers)
		{
			trainer->ExecuteAction();
		}
	}
}

void TestingState::Draw()
{
	ui->draw();
	eng.Draw(engDrawRec);
	DrawRectangleLinesEx(engDrawRec, 5, app.config.ui.secondaryColor);
}

void TestingState::LoadTrainer(std::string modelName, size_t totalTrainers)
{
	double angle = 2 * std::numbers::pi * envs.size() / totalTrainers;

	NeuralWarfareEnv* env = new NeuralWarfareEnv(eng,
		eng.AddTeam(app.config.engine.teamSize, app.config.engine.agentBaseHealth, Vec2{
		static_cast<float>(app.config.engine.sizeX * 0.5 * cos(angle)) ,
		static_cast<float>(app.config.engine.sizeY * 0.5 * sin(angle)) 
		} * -1
	));
	envs.push_back(env);

	std::filesystem::path path = app.config.filePaths.modelFolder / MakeFilename(modelName, "bin");
	if (!std::filesystem::exists(path))
	{
		std::cerr << "ERROR: File '"<< path.filename().string().c_str() << "' does not exist, model has likely been removed or renamed by an external entity";
		app.ChangeState(EgameState::TESTSELECTION);
		return;
	}
	NeuralNetwork* network = NeuralNetwork::Load(functions, path);
	Trainer* trainer = new StaticNNTrainer(env, network);

	trainers.push_back(trainer);
	float newEntryXpos;
	if (trainerlList->childElements.empty())
	{
		newEntryXpos = modelListEntryXPos;
	}
	else
	{
		newEntryXpos = trainerlList->childElements.back()->getPos().x + modelListEntryHSpaceing;
	}

	Rectangle rec = {
		newEntryXpos,
		modelListEntryYPos,
		modelListEntryWidth,
		modelListEntryHeight
	};
	TrainerListEntry* tle = new TrainerListEntry(trainerlList, rec, trainer, modelName);

	new UILiveText<UITextLine>{ tle,
		[env]() { return "Total Kills - This Episode : " + std::to_string(env->GetTotalKillsThisEpisode()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.4f }, "", rec.height * 0.12f
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Highest Kills - This Episode : " + std::to_string(env->GetHighestKillsThisEpisode()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.55f }, "", rec.height * 0.12f
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Total Kills - All Episodes : " + std::to_string(env->GetTotalKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.7f }, "", rec.height * 0.12f
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Highest Kills - All Episodes : " + std::to_string(env->GetHighestKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.85f }, "", rec.height * 0.12f
	};
}

void TestingState::FindBestTrainer()
{
	size_t bestTrainerTotalKills = 0;
	std::string bestTrainerModelName = "";
	for (UIElement* element : trainerlList->childElements)
	{
		TrainerListEntry* tle = dynamic_cast<TrainerListEntry*>(element);
		if (tle)
		{
			size_t trainerTotalKills;
			if ((trainerTotalKills = static_cast<NeuralWarfareEnv*>(tle->trainer->env)->GetTotalKillsAllEpisodes()) > bestTrainerTotalKills)
			{
				bestTrainerTotalKills = trainerTotalKills;
				bestTrainerModelName = tle->nameText->GetText();
				bestTrainerListEntry->trainer = tle->trainer;
			}
		}
		else
		{
			std::cerr << "ERROR: Failed to convert UIElement TrainerListEntry\n";
		}
	}
	bestTrainerListEntry->nameText->SetText(bestTrainerModelName);
}
