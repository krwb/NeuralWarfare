#include "TestSelectionState.h"
#include "Application.h"
#include "NeuralNetwork.h"
TestSelectionState::TestSelectionState(Application& app) : GameState(app)
{
	ui = new UIContainer(app.config.ui.primaryColor, app.config.ui.secondaryColor, app.config.ui.textColor);

	modelListEntryYPos = app.config.app.screenHeight * 0.126f;
	modelListEntryWidth = app.config.app.screenWidth * 0.3f;
	modelListEntryHeight = app.config.app.screenHeight * 0.15f;
	modelListEntryVSpaceing = modelListEntryHeight * 0.6;
	modelListEntryInputTextSize = app.config.app.screenHeight * 0.025f;

	modelListEntryXPos = app.config.app.screenWidth * 0.135f;
	selectedModelListEntryXPos = app.config.app.screenWidth * 0.865f - modelListEntryWidth;

	modelList = new UISliderContainer(ui, {
		app.config.app.screenWidth * 0.1f, app.config.app.screenHeight * 0.14f,
		app.config.app.screenWidth * 0.025f, app.config.app.screenHeight * 0.62f
		}, 10
	);
	new UITextLine(ui, { modelListEntryXPos + modelListEntryWidth * 0.5f,modelListEntryYPos - modelListEntryHeight * 0.2f }, "Avalable Models", modelListEntryHeight * 0.25);

	selectedModelList = new UISliderContainer(ui, {
		app.config.app.screenWidth * 0.875f, app.config.app.screenHeight * 0.14f,
		app.config.app.screenWidth * 0.025f, app.config.app.screenHeight * 0.62f
		}, 10
	);
	new UITextLine(ui, { selectedModelListEntryXPos + modelListEntryWidth * 0.5f,modelListEntryYPos - modelListEntryHeight * 0.2f }, "Selected Models", modelListEntryHeight * 0.3);

	UITextLine* selectPrompt = new UITextLine(ui, { app.config.app.screenWidth * 0.5f, app.config.app.screenHeight * 0.8f }, "You must select at least 2 models to test", app.config.app.screenHeight * 0.025f);
	selectPrompt->hidden = true;

	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "FIGHT", app.config.app.screenHeight * 0.1f,
		[this,selectPrompt]()
		{
			if (this->selectedModelList->childElements.size() >= 2)
			{
				this->app.ChangeState(EgameState::TESTING);
			}
			else
			{
				selectPrompt->hidden = false;
			}
		},
		Rectangle{app.config.app.screenWidth * 0.3f, app.config.app.screenHeight * 0.825f, app.config.app.screenWidth * 0.4f, app.config.app.screenHeight * 0.15f}
	};

	GetModels();
	new UILabeledButton<UIFunctionButton<UIButtonRec>>{
		ui, "BACK", app.config.app.screenHeight * 0.05f,
		[&app]() { app.ChangeState(EgameState::MAINMENU); },
		Rectangle{app.config.app.screenWidth * 0.01f, app.config.app.screenHeight * 0.01f, app.config.app.screenWidth * 0.1f, app.config.app.screenHeight * 0.05f}
	};
}

TestSelectionState::~TestSelectionState()
{
	delete ui;
}

void TestSelectionState::Load()
{
}

void TestSelectionState::Unload()
{
}

void TestSelectionState::Update(float deltaTime)
{
	ui->update();
	RemoveDeselectedModels();
}

void TestSelectionState::Draw()
{
	ui->draw();
}

void TestSelectionState::AddModel(ModelListEntry* modelEntry)
{
	selectedModelList->value = 0;
	selectedModelList->griped = false;
	selectedModelList->update();
	float newEntryY;
	if (selectedModelList->childElements.empty())
	{
		newEntryY = modelListEntryYPos;
	}
	else
	{
		newEntryY = selectedModelList->childElements.back()->getPos().y + modelListEntryVSpaceing;
	}
	ModelListEntry* newEntry = new ModelListEntry(selectedModelList, Rectangle{ selectedModelListEntryXPos, newEntryY , modelListEntryWidth, modelListEntryHeight }, *modelEntry);
	new UILabeledButton<UIFunctionButton<UIButtonRec>>(
		newEntry, "Deselect", modelListEntryHeight * 0.1f,
		[this, newEntry]() { this->RemoveModel(newEntry); },
		Rectangle{
			selectedModelListEntryXPos + modelListEntryWidth * 0.55f,
			newEntryY + modelListEntryHeight * 0.75f,
			modelListEntryWidth * 0.4f,
			modelListEntryHeight * 0.2f
		}
	);
}

void TestSelectionState::RemoveModel(ModelListEntry* modelEntry)
{
	modelsForRemoval.push_back(modelEntry);
}

void TestSelectionState::RemoveDeselectedModels()
{
	if (modelsForRemoval.empty()) return;

	while (!modelsForRemoval.empty())
	{
		delete modelsForRemoval.back();
		modelsForRemoval.pop_back();
	}
	selectedModelList->value = 0;
	selectedModelList->griped = false;
	selectedModelList->update();
	std::list<UIElement*>::iterator elementIter = selectedModelList->childElements.begin();
	while (elementIter != selectedModelList->childElements.end())
	{
		float newEntryY;
		if (elementIter == selectedModelList->childElements.begin())
		{
			newEntryY = modelListEntryYPos;
		}
		else
		{
			newEntryY = (*std::prev(elementIter))->getPos().y + modelListEntryVSpaceing;
		}
		newEntryY += modelListEntryHeight * 0.5;
		(*elementIter)->setPos({ (*elementIter)->getPos().x,newEntryY });
		elementIter++;
	}
}

std::string FormatFileSize(std::uintmax_t fileSize) {
	const double KB = 1024.0;
	const double MB = KB * 1024.0;
	const double GB = MB * 1024.0;

	double sizeInUnits;
	std::string unit;

	if (fileSize >= GB) {
		sizeInUnits = fileSize / GB;
		unit = "GB";
	}
	else if (fileSize >= MB) {
		sizeInUnits = fileSize / MB;
		unit = "MB";
	}
	else if (fileSize >= KB) {
		sizeInUnits = fileSize / KB;
		unit = "KB";
	}
	else {
		sizeInUnits = fileSize;
		unit = "bytes";
	}

	std::ostringstream out;
	out << std::fixed << std::setprecision(2) << sizeInUnits;
	std::string str = out.str();

	str.erase(str.find_last_not_of('0') + 1, std::string::npos);

	if (str.back() == '.') {
		str.pop_back();
	}

	return str + " " + unit;
}

void TestSelectionState::GetModels()
{
	std::vector<ActivationFunction*> functions;
	for (const auto& file : std::filesystem::directory_iterator(std::filesystem::current_path() / app.config.filePaths.modelFolder)) {
		if (std::filesystem::is_regular_file(file.status())) {
			
			float newEntryY;
			if (modelList->childElements.empty())
			{
				newEntryY = modelListEntryYPos;
			}
			else
			{
				newEntryY = modelList->childElements.back()->getPos().y + modelListEntryVSpaceing;
			}
			ModelListEntry* newEntry = new ModelListEntry(modelList,
				Rectangle{
					modelListEntryXPos, newEntryY ,
					modelListEntryWidth, modelListEntryHeight
				},
				file.path().stem().string(),
				std::string(
					"Filesize: " + FormatFileSize(file.file_size()) + "\n"
					)
			);

			new UILabeledButton<UIFunctionButton<UIButtonRec>>(
				newEntry, "Select", modelListEntryHeight * 0.1f,
				[this, newEntry]() { this->AddModel(newEntry); },
				Rectangle{
					modelListEntryXPos + modelListEntryWidth * 0.55f,
					newEntryY + modelListEntryHeight * 0.75f,
					modelListEntryWidth * 0.4f,
					modelListEntryHeight * 0.2f
				}
			);
		}
	}
}

ModelListEntry::ModelListEntry(UIContainer* parentContainer, Rectangle rec, std::string modelName, std::string details) : UIElement(parentContainer), UISubContainer(parentContainer), UIRectangle(parentContainer, rec)
{
	nameText = new UITextLine(this, { rec.x + rec.width * 0.5f, rec.y + rec.height * 0.15f }, modelName, rec.height * 0.2);
	detailsText = new UIBackgroundlessTextBox(this, Rectangle{ rec.x + rec.width * 0.02f,rec.y + rec.height * 0.255f,rec.width * 0.96f ,rec.height * 0.6f }, details, rec.height * 0.1f);
}

ModelListEntry::ModelListEntry(UIContainer* parentContainer, Rectangle rec, ModelListEntry& listEntry) : UIElement(parentContainer), UISubContainer(parentContainer), UIRectangle(parentContainer, rec)
{
	nameText = new UITextLine(this, { rec.x + rec.width * 0.5f, rec.y + rec.height * 0.2f }, listEntry.nameText->GetText(), rec.height * 0.2);
	detailsText = new UIBackgroundlessTextBox(this, Rectangle{ rec.x + rec.width * 0.02f,rec.y + rec.height * 0.255f,rec.width * 0.96f ,rec.height * 0.6f }, listEntry.detailsText->GetText() , rec.height * 0.1f);

}

ModelListEntry::~ModelListEntry()
{
}

void ModelListEntry::draw(float alpha)
{
	UIRectangle::draw(alpha - 100);
	UIContainer::draw(alpha);
}

void ModelListEntry::update()
{
	UIRectangle::update();
	UIContainer::update();
}

void ModelListEntry::setPos(const Vec2& pos)
{
	Vec2 move = getPos() - pos;
	UIRectangle::setPos(pos);
	for (UIElement* e : childElements)
	{
		e->setPos(e->getPos() - move);
	}
}
