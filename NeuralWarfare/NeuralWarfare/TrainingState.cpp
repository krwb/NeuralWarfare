#include "TrainingState.h"
#include "Application.h"

TrainingState::TrainingState(Application& app) : GameState(app), eng(app.gen, { app.config.engine.sizeX,app.config.engine.sizeY }), engDrawRec({}), netVis(nullptr, {})
{
	netVis.drawRec = {
	app.config.app.screenWidth * 0.71f,
	app.config.app.screenHeight * 0.14f,
	app.config.app.screenWidth * 0.2f,
	app.config.app.screenHeight * 0.25f
	};

    engDrawRec = { 
		app.config.app.screenWidth * 0.25f, 
		app.config.app.screenHeight * 0.28f,
        app.config.app.screenWidth * 0.73f, 
        app.config.app.screenHeight * 0.70f 
	};

	functions.push_back(&addfunction);
	functions.push_back(&sigmoidFunction);
	functions.push_back(&tanhFunction);

	ui = new UIContainer(app.config.ui.primaryColor, app.config.ui.secondaryColor, app.config.ui.textColor);
	trainerList = new UISliderContainer(ui, {
		app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.25f,
		app.config.app.screenWidth * 0.025f, app.config.app.screenHeight * 0.54f
		}, 10
	);

	hyperparameterControls = new UISliderContainer(ui, {
		app.config.app.screenWidth * 0.25f, 
		app.config.app.screenHeight * 0.015f,
		app.config.app.screenWidth * 0.025f,
		app.config.app.screenHeight * 0.25f
		}, 15.5
	);
	float hyperparameterControlLabelXPos = app.config.app.screenWidth * 0.285f;
	float hyperparameterControlInputXPos = app.config.app.screenWidth * 0.445f;
	float hyperparameterControlWidth = app.config.app.screenWidth * 0.15f;
	float hyperparameterControlHeight = app.config.app.screenHeight * 0.05f;
	float hyperparameterControlVSpaceing = hyperparameterControlHeight * 0.8;
	float hyperparameterControlInputTextSize = app.config.app.screenHeight * 0.025f;
	float hyperparameterControlLabelTextSize = app.config.app.screenHeight * 0.015f;
	topAgentCountInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls,Rectangle{ hyperparameterControlInputXPos , app.config.app.screenHeight * 0.016f, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Top Agent Count", hyperparameterControlLabelTextSize);
	mutationCountInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Mutation Count", hyperparameterControlLabelTextSize);
	biasMutationRateInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Bias Mutation Rate", hyperparameterControlLabelTextSize);
	biasMutationMagnitudeInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Bias Mutation Magnitude", hyperparameterControlLabelTextSize);
	weightMutationRateInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Weight Mutation Rate", hyperparameterControlLabelTextSize);
	weightMutationMagnitudeInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Weight Mutation Magnitude", hyperparameterControlLabelTextSize);
	synapseMutationRateInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Synapse Mutation Rate", hyperparameterControlLabelTextSize);
	newSynapseMagnitudeInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "New Synapse Magnitude", hyperparameterControlLabelTextSize);
	nodeMutationRateInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Node Mutation Rate", hyperparameterControlLabelTextSize);
	layerMutationRateInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "Layer Mutation Rate", hyperparameterControlLabelTextSize);
	newLayerSizeAverageInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "New Layer Size Average", hyperparameterControlLabelTextSize);
	newLayerSizeRangeInput = new UITextInput<UIBackgroundlessTextBox>(hyperparameterControls, Rectangle{ hyperparameterControlInputXPos , hyperparameterControls->childElements.back()->getPos().y + hyperparameterControlVSpaceing, hyperparameterControlWidth , hyperparameterControlHeight }, "", hyperparameterControlInputTextSize);
	new UIBackgroundlessTextBox(hyperparameterControls, Rectangle{ hyperparameterControlLabelXPos , hyperparameterControls->childElements.back()->getPos().y - hyperparameterControlHeight / 2,hyperparameterControlWidth,hyperparameterControlHeight }, "New Layer Size Range", hyperparameterControlLabelTextSize);

	// Speed control buttons

	// Button size
	float buttonWidth = app.config.app.screenWidth * 0.05f;
	float buttonHeight = buttonWidth;

	// Padding
	float paddingX = (app.config.app.screenWidth * 0.25 - (4 * buttonWidth)) * 0.2f;
	float paddingY = (app.config.app.screenHeight * 0.25 - (2 * buttonHeight)) * 0.1f;

	size_t* stepsPerFramePointer = &stepsPerFrame;
	// Create 8 buttons
	for (int i = 0; i < 8; ++i) {
		float x = (i % 4) * (buttonWidth + paddingX) + paddingX;
		float y = (i / 4) * (buttonHeight + paddingY) + app.config.app.screenHeight * 0.815f;
		size_t buttonSpeedMult = (size_t)std::round(std::pow(i + 1, 2) / 2);
		new UILabeledButton<UIFunctionButton<UIButtonRec>>{
			ui, std::to_string(buttonSpeedMult) + "x", buttonHeight * 0.5f,
			[stepsPerFramePointer, buttonSpeedMult]() { *stepsPerFramePointer = buttonSpeedMult; },
			Rectangle{x, y, buttonWidth, buttonHeight}
		};
	}
	
	nameInput = new UITextInput<UIBackgroundlessTextBox>(ui, Rectangle{
		app.config.app.screenWidth * 0.82f, app.config.app.screenHeight * 0.035f, 
		app.config.app.screenWidth * 0.16f, app.config.app.screenHeight * 0.05f 
		}, "", app.config.app.screenHeight * 0.025f
	);

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "Save Model", app.config.app.screenHeight * 0.025f,
		[this]() { this->SaveSelectedModel(); },
		Rectangle{ app.config.app.screenWidth * 0.82f, app.config.app.screenHeight * 0.215f,
		app.config.app.screenWidth * 0.16f, app.config.app.screenHeight * 0.05f
		}
	};
	
	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
	ui, "Start Training", app.config.app.screenHeight * 0.025f,
	[this]() { if (this->selectedTrainer) this->selectedTrainer->trainer->training = true; },
	Rectangle{app.config.app.screenWidth * 0.82f, app.config.app.screenHeight * 0.095f, app.config.app.screenWidth * 0.16f, app.config.app.screenHeight * 0.05f}
	};

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "Stop Training", app.config.app.screenHeight * 0.025f,
		[this]() { if (this->selectedTrainer) this->selectedTrainer->trainer->training = false; },
		Rectangle{app.config.app.screenWidth * 0.82f, app.config.app.screenHeight * 0.155f, app.config.app.screenWidth * 0.16f, app.config.app.screenHeight * 0.05f}
	};

	new UITextLine(ui, { app.config.app.screenWidth * 0.12f, app.config.app.screenHeight * 0.15f }, "Loaded Models", app.config.app.screenHeight * 0.03);

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "New", app.config.app.screenHeight * 0.05f,
		[this]() { this->AddNewModel(); },
		Rectangle{app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.175f, app.config.app.screenWidth * 0.11f, app.config.app.screenHeight * 0.05f}
	};
	
	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "Load", app.config.app.screenHeight * 0.05f,
		[this]() { this->SetLoadPopupHidden(false); },
		Rectangle{app.config.app.screenWidth * 0.13f, app.config.app.screenHeight * 0.175f, app.config.app.screenWidth * 0.11f, app.config.app.screenHeight * 0.05f}
	};

	(loadModelInput = new UIPopup(ui, Rectangle{ 0,0,app.config.app.screenWidth * 0.3f,app.config.app.screenHeight * 0.2f }))->setPos({ app.config.app.screenWidth * 0.5f, app.config.app.screenHeight * 0.5f });
	loadModelInput->hidden = true;

	new UITextLine(loadModelInput, { app.config.app.screenWidth * 0.5f, app.config.app.screenHeight * 0.45f },"Load Model",app.config.app.screenHeight * 0.05f);
	new UITextLine(loadModelInput, { app.config.app.screenWidth * 0.5f, app.config.app.screenHeight * 0.5f }, "Input Model Name", app.config.app.screenHeight * 0.025f);
	loadNameInput = new UITextInput<UIBackgroundlessTextBox>(loadModelInput, Rectangle{
		app.config.app.screenWidth * 0.4f, app.config.app.screenHeight * 0.525f,
		app.config.app.screenWidth * 0.2f, app.config.app.screenHeight * 0.05f
		}
	);

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		loadModelInput, "X", app.config.app.screenHeight * 0.025f,
		[this]() { this->SetLoadPopupHidden(true); },
		Rectangle{app.config.app.screenWidth * 0.63f, app.config.app.screenHeight * 0.405f, app.config.app.screenHeight * 0.025f, app.config.app.screenHeight * 0.025f}
	};

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
	ui, "BACK", app.config.app.screenHeight * 0.05f,
	[&app]() { app.ChangeState(EgameState::MAINMENU); },
	Rectangle{app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.01f, app.config.app.screenWidth * 0.1f, app.config.app.screenHeight * 0.05f}
	};
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
	for (NeuralWarfareEnv* env : envs)
	{
		env->UpdateKillTrackers();
	}
    ui->update();
	UpdateHyperparameterControls();
	for (size_t i = 0; i < stepsPerFrame; i++)
	{
		resetTimer += 1.0f / 60.0f;//deltaTime;

		if (resetTimer > 5)
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
		eng.Update(4);
		trainerFuture->wait();

		for (Trainer* trainer : trainers)
		{
			trainer->ExecuteAction();
		}
	}

	if (selectedTrainer)
	{
		if (nameInput->IsSubmited())
		{
			selectedTrainer->nameText->SetText(nameInput->GetText());
		}
		else if (!nameInput->selected)
		{
			nameInput->SetText(selectedTrainer->nameText->GetText());
		}
		netVis.network = static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->masterNetwork;
	}
	if (!loadModelInput->hidden && loadNameInput->IsSubmited())
	{
		LoadModel(loadNameInput->GetText());
		SetLoadPopupHidden(true);
	}
}

void TrainingState::Draw()
{
    ui->draw();
	eng.Draw(engDrawRec);
    DrawRectangleLinesEx(engDrawRec, 5, app.config.ui.secondaryColor);
	DrawRectangleRec({netVis.drawRec.x - netVis.drawRec.width * 0.5f,netVis.drawRec.y - netVis.drawRec.height * 0.5f ,netVis.drawRec.width,netVis.drawRec.height}, app.config.ui.secondaryColor);
	netVis.Draw();
}

void TrainingState::SetSelectedTrainer(TrainerListEntry* trainerListEntry)
{
	selectedTrainer = trainerListEntry;
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
	AddTrainer(network,"UnnamedModel");
}

void TrainingState::LoadModel(std::string modelName)
{
	std::filesystem::path modelPath = std::filesystem::current_path() / app.config.filePaths.modelFolder / MakeFilename(modelName,"bin");
	if (!std::filesystem::exists(modelPath)) {
		std::cerr << "Error: Model '" << modelName << "' not found in model folder\n";
		return;
	}
	AddTrainer(NeuralNetwork::Load(functions, modelPath),modelName);
	std::cerr << "Loaded model '" << modelName << "'\n";
}

void TrainingState::SetLoadPopupHidden(bool hidden)
{
	loadNameInput->SetText("");
	loadModelInput->hidden = hidden;
}

void TrainingState::AddTrainer(NeuralNetwork* network,std::string modelName)
{
	GeneticAlgorithmNNTrainer::MyHyperparameters hyperperameters("hyperperameters.xml");
	NeuralWarfareEnv* env = new NeuralWarfareEnv(eng,
		eng.AddTeam(app.config.engine.teamSize, app.config.engine.agentBaseHealth, { 0,0 }
		));

	envs.push_back(env);
	GeneticAlgorithmNNTrainer* trainer = new GeneticAlgorithmNNTrainer(env, app.gen, hyperperameters, network);
	trainers.push_back(trainer);

	if (envs.size() > 1)
	{
		for (size_t i = 0; i < envs.size(); i++)
		{
			double angle = 2 * std::numbers::pi * i / envs.size();
			envs[i]->SetTeamSpawnPos({ static_cast<float>(app.config.engine.sizeX * 0.5 * cos(angle)) , static_cast<float>(app.config.engine.sizeY * 0.5 * sin(angle)) });
		}
	}

	Vec2 newEntryPos;
	trainerList->value = 0;
	trainerList->griped = false;
	trainerList->update();
	if (trainerList->childElements.empty())
	{
		newEntryPos = { app.config.app.screenWidth * 0.045f, app.config.app.screenHeight * 0.25f };
	}
	else
	{
		newEntryPos = { app.config.app.screenWidth * 0.045f, trainerList->childElements.back()->getPos().y + app.config.app.screenHeight * 0.095f };
	}
	Rectangle rec = { newEntryPos.x,newEntryPos.y ,app.config.app.screenWidth * 0.195f ,app.config.app.screenHeight * 0.175f };
	TrainerListEntry* tle = new TrainerListEntry(trainerList, rec, trainer, modelName);
	float textSize = app.config.app.screenHeight * 0.015f;
	SetSelectedTrainer(tle);
	tle->nameText->SetSize(app.config.app.screenHeight * 0.03f);
	tle->nameText->setPos(tle->nameText->getPos() - Vec2{ 0 , app.config.app.screenHeight * 0.01f });
	new UILiveText<UITextLine>{ tle,
	[env]() { return "Total Kills - This Episode : " + std::to_string(env->GetTotalKillsThisEpisode()); },
	Vec2{ rec.x + rec.width * 0.5f ,rec.y + app.config.app.screenHeight * 0.0555f }, "", textSize
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Highest Kills - This Episode : " + std::to_string(env->GetHighestKillsThisEpisode()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + app.config.app.screenHeight * 0.078f }, "", textSize
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Total Kills - All Episodes : " + std::to_string(env->GetTotalKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + app.config.app.screenHeight * 0.1005f }, "", textSize
	};
	new UILiveText<UITextLine>{ tle,
		[env]() { return "Highest Kills - All Episodes : " + std::to_string(env->GetHighestKillsAllEpisodes()); },
		Vec2{ rec.x + rec.width * 0.5f ,rec.y + app.config.app.screenHeight * 0.123f }, "", textSize
	};
	new UILiveText<UITextLine>{
		tle,
		[trainer]() { return trainer->training ? "Training..." : "Not Training"; },
		Vec2{ rec.x + rec.width * 0.25f ,rec.y + app.config.app.screenHeight * 0.1525f }, "", textSize
	};
	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
	tle, "Select", rec.height * 0.1f,
	[this,tle]() { this->SetSelectedTrainer(tle); },
	Rectangle{rec.x + rec.width * 0.55f, rec.y + app.config.app.screenHeight * 0.1375f, rec.width * 0.4f, rec.height * 0.2f}
	};
}

void TrainingState::SaveSelectedModel()
{
	if (selectedTrainer)
	{
		std::filesystem::path modelFolder = std::filesystem::current_path() / app.config.filePaths.modelFolder;
		if (!std::filesystem::exists(modelFolder)) {
			std::filesystem::create_directory(modelFolder);
		}
		NeuralNetwork::Save(*static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->masterNetwork,
			modelFolder / MakeFilename(selectedTrainer->nameText->GetText(), "bin")
		);
		std::cerr << "Model saved to: " << modelFolder.string().c_str() << std::endl;
	}
}

std::string floatToString(float value) {
	std::ostringstream out;
	out << std::fixed << value;
	std::string str = out.str();

	// Find the decimal point
	size_t decimalPoint = str.find('.');
	if (decimalPoint != std::string::npos) {
		// Remove trailing zeros
		str.erase(str.find_last_not_of('0') + 1, std::string::npos);

		// Remove the decimal point if it's the last character
		if (str.back() == '.') {
			str.pop_back();
		}
	}

	return str;
}

void TrainingState::UpdateHyperparameterControls()
{
	if (!selectedTrainer) return;

	try
	{
		if (topAgentCountInput->IsSubmited())  static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.topAgentCount = abs(std::stoi(topAgentCountInput->GetText()));
		if (mutationCountInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.mutationCount = abs(std::stoi(mutationCountInput->GetText()));
		if (biasMutationRateInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.biasMutationRate = std::stof(biasMutationRateInput->GetText());
		if (biasMutationMagnitudeInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.biasMutationMagnitude = std::stof(biasMutationMagnitudeInput->GetText());
		if (weightMutationRateInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.weightMutationRate = std::stof(weightMutationRateInput->GetText());
		if (weightMutationMagnitudeInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.weightMutationMagnitude = std::stof(weightMutationMagnitudeInput->GetText());
		if (synapseMutationRateInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.synapseMutationRate = std::stof(synapseMutationRateInput->GetText());
		if (newSynapseMagnitudeInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newSynapseMagnitude = std::stof(newSynapseMagnitudeInput->GetText());
		if (nodeMutationRateInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.nodeMutationRate = std::stof(nodeMutationRateInput->GetText());
		if (layerMutationRateInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.layerMutationRate = std::stof(layerMutationRateInput->GetText());
		if (newLayerSizeAverageInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newLayerSizeAverage = abs(std::stoi(newLayerSizeAverageInput->GetText()));
		if (newLayerSizeRangeInput->IsSubmited()) static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newLayerSizeRange = abs(std::stoi(newLayerSizeRangeInput->GetText()));

	}
	catch (const std::invalid_argument&)
	{
		std::cerr << "Error: Invalid string conversion";
	}
	if (!topAgentCountInput->selected) topAgentCountInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.topAgentCount));
	if (!mutationCountInput->selected) mutationCountInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.mutationCount));
	if (!biasMutationRateInput->selected) biasMutationRateInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.biasMutationRate));
	if (!biasMutationMagnitudeInput->selected) biasMutationMagnitudeInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.biasMutationMagnitude));
	if (!weightMutationRateInput->selected) weightMutationRateInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.weightMutationRate));
	if (!weightMutationMagnitudeInput->selected) weightMutationMagnitudeInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.weightMutationMagnitude));
	if (!synapseMutationRateInput->selected) synapseMutationRateInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.synapseMutationRate));
	if (!newSynapseMagnitudeInput->selected) newSynapseMagnitudeInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newSynapseMagnitude));
	if (!nodeMutationRateInput->selected) nodeMutationRateInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.nodeMutationRate));
	if (!layerMutationRateInput->selected) layerMutationRateInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.layerMutationRate));
	if (!newLayerSizeAverageInput->selected) newLayerSizeAverageInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newLayerSizeAverage));
	if (!newLayerSizeRangeInput->selected) newLayerSizeRangeInput->SetText(floatToString(static_cast<GeneticAlgorithmNNTrainer*>(selectedTrainer->trainer)->hyperparameters.newLayerSizeRange));

}

TrainerListEntry::TrainerListEntry(UIContainer* parentContainer, Rectangle rec, Trainer* trainer, std::string modelName) : UIElement(parentContainer), UISubContainer(parentContainer), UIRectangle(parentContainer, rec), trainer(trainer)
{
	nameText = new UITextLine(this, { rec.x + rec.width * 0.5f ,rec.y + rec.height * 0.2f }, modelName, rec.height * 0.2f);
}


TrainerListEntry::~TrainerListEntry()
{
}

void TrainerListEntry::draw(float alpha)
{
	UIRectangle::draw(alpha - 100);
	Color color = NeuralWarfareEngine::GenerateTeamColor(static_cast<NeuralWarfareEnv*>(trainer->env)->teamId);
	float ta = static_cast<float>(color.a) + alpha;
	color.a = ta > 0 ? ta < 255 ? ta : 255 : 0;
	DrawCircle(rec.x + rec.width * 0.9f, rec.y + rec.height * 0.1f, rec.height * 0.05f,color);
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
		e->setPos(e->getPos() - move);
	}
}