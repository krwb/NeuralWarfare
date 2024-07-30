#pragma once
#include <iostream>
#include <string>
#include "raylib.h"
#include "tinyxml2.h"
#include <filesystem>

/// <summary>
/// helper function to ensure filenames have the correct extension
/// </summary>
/// <param name="filename">the filename with or without the extension</param>
/// <param name="extension">the extension the file should have</param>
/// <returns>filename with extension </returns>
static std::string MakeFilename(const std::string& filename, const std::string& extension) {
	std::string correctExtension = (extension[0] == '.') ? extension : "." + extension;
	size_t lastDot = filename.find_last_of('.');
	std::string baseFilename = (lastDot != std::string::npos) ? filename.substr(0, lastDot) : filename;
	return baseFilename + correctExtension;
}

/// <summary>
/// class for managing configs
/// </summary>
class Config
{
public:
	struct App
	{
		int screenWidth = 1200;
		int screenHeight = 800;
		int targetFPS = 60;
	};
	App app;
	struct UI
	{
		Color backgroundColor = WHITE;
		Color primaryColor = GRAY;
		Color secondaryColor = LIGHTGRAY;
		Color textColor = BLACK;
		float fpsTextSize = 20;
	};
	UI ui;
	struct FilePaths
	{
		std::filesystem::path configPath;
		std::filesystem::path modelFolder = "models";
	};
	FilePaths filePaths;
	struct Engine
	{
		float sizeX = 550;
		float sizeY = 350;;
		size_t teamSize = 100;
		float agentBaseHealth = 2;
		float updateDelta = 4;
		float resetTime = 5;
	};
	Engine engine;

	struct HyperparameterCap
	{
		size_t topAgentCount;
		size_t mutationCount = 100;
		float biasMutationRate = 1;
		float biasMutationMagnitude = 0;
		float weightMutationRate = 1;
		float weightMutationMagnitude = 0;
		float synapseMutationRate = 1;
		float newSynapseMagnitude = 0;
		float nodeMutationRate = 1;
		float layerMutationRate = 1;
		size_t newLayerSizeAverage = 5;
		size_t newLayerSizeRange = 3;
	};
	HyperparameterCap hyperparameterCap;

	Config(std::string configPath)
	{
		filePaths.configPath = configPath;
		Load();
	}

	/// <summary>
	/// Helper function to load colors from XML document
	/// </summary>
	/// <param name="root">the element containing the color element </param>
	/// <param name="name"> the name the color is saved under</param>
	/// <param name="value"> pointer to color value to be set</param>
	/// <returns> and tinyxml2::XMLErrors returned when reading elements</returns>
	static tinyxml2::XMLError LoadColor(tinyxml2::XMLElement* element, std::string name, Color* value)
	{
		// Loading backgroundColor
		tinyxml2::XMLError e = tinyxml2::XML_SUCCESS;
		tinyxml2::XMLElement* colorElement = element->FirstChildElement(name.c_str());
		if (colorElement)
		{
			int intValue;

			if ((e = colorElement->QueryIntAttribute("r", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->r = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "ERROR: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->g = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "ERROR: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->b = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "ERROR: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->a = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "ERROR: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
		}
		else
		{
			std::cerr << "ERROR: Color Element '" << element->Name() << "." << name << "'  not found in the configuration file." << std::endl;
		}
		return e;
	}

	/// <summary>
	/// load the configs from file
	/// </summary>
	void Load()
	{
		std::cerr << "INFO: Loading NeuralWarfare configs..." << std::endl;

		tinyxml2::XMLError e;
		tinyxml2::XMLDocument doc;
		{
			if ((e = doc.LoadFile(filePaths.configPath.string().c_str())) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "ERROR loading XML file: " << filePaths.configPath << "TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				return;
			}
		}

		tinyxml2::XMLElement* root = doc.RootElement();
		if (!root) 
		{
			std::cerr << "ERROR: No root element found in XML file." << std::endl;
			return;
		}

		tinyxml2::XMLElement* appElement = root->FirstChildElement("App");
		if (appElement) 
		{
			if ((e = appElement->QueryIntAttribute("screenWidth", &app.screenWidth)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'app.screenWidth' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'app.screenWidth'" << std::endl;
			if ((e = appElement->QueryIntAttribute("screenHeight", &app.screenHeight)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'app.screenHeight' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'app.screenHeight'" << std::endl;
			if ((e = appElement->QueryIntAttribute("targetFPS", &app.targetFPS)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'app.targetFPS' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'app.targetFPS'" << std::endl;
		}
		else
		{
			std::cerr << "ERROR: 'App' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* uiElement = root->FirstChildElement("UI");
		if (uiElement) 
		{
			// Loading backgroundColor
			if ((e = LoadColor(uiElement,"BackgroundColor",&ui.backgroundColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "ERROR: Failed to load color 'ui.backgroundColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
			else
			{
				std::cerr << "INFO: Loaded color 'ui.backgroundColor'" << std::endl;
			}
			// Loading primaryColor
			if ((e = LoadColor(uiElement, "PrimaryColor", &ui.primaryColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "ERROR: Failed to load color 'ui.primaryColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
			else
			{
				std::cerr << "INFO: Loaded color 'ui.primaryColor'" << std::endl;
			}

			// Loading secondaryColor
			if ((e = LoadColor(uiElement, "SecondaryColor", &ui.secondaryColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "ERROR: Failed to load color 'ui.secondaryColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
			else
			{
				std::cerr << "INFO: Loaded color 'ui.secondaryColor'" << std::endl;
			}

			// Loading textColor
			if ((e = LoadColor(uiElement, "TextColor", &ui.textColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "ERROR: Failed to load color 'ui.textColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
			else
			{
				std::cerr << "INFO: Loaded color 'ui.textColor'" << std::endl;
			}

			// Loading baseTextSize
			if ((e = uiElement->QueryFloatAttribute("FPSTextSize", &ui.fpsTextSize)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'ui.fpsTextSize' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'ui.fpsTextSize'";

		}
		else
		{
			std::cerr << "ERROR: 'UI' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* filePathsElement = root->FirstChildElement("FilePaths");
		if (filePathsElement)
		{
			filePaths.modelFolder = filePathsElement->Attribute("ModelFolder");
		}
		else
		{
			std::cerr << "ERROR: 'FilePaths' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* engineElement = root->FirstChildElement("Engine");
		if (engineElement)
		{
			if ((e = engineElement->QueryFloatAttribute("SizeX", &engine.sizeX)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.sizeX' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.sizeX'" << std::endl;
			if ((e = engineElement->QueryFloatAttribute("SizeY", &engine.sizeY)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.sizeY' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.sizeY'" << std::endl;
			if ((e = engineElement->QueryUnsigned64Attribute("TeamSize", &engine.teamSize)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.teamSize' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.teamSize'" << std::endl;
			if ((e = engineElement->QueryFloatAttribute("AgentBaseHealth", &engine.agentBaseHealth)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.agentBaseHealth' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.agentBaseHealth'" << std::endl;
			if ((e = engineElement->QueryFloatAttribute("UpdateDelta", &engine.updateDelta)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.updateDelta' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.updateDelta'" << std::endl;
			if ((e = engineElement->QueryFloatAttribute("ResetTime", &engine.resetTime)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'engine.resetTime' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'engine.resetTime'" << std::endl;

		}
		else
		{
			std::cerr << "ERROR: 'Engine' element not found in the configuration file." << std::endl;
		}
		tinyxml2::XMLElement* hyperparameterCapElement = root->FirstChildElement("HyperparameterCap");
		if (hyperparameterCapElement)
		{
			//if ((e = hyperparameterCapElement->QueryUnsigned64Attribute("TopAgentCount", &hyperparameterCap.topAgentCount)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.topAgentCount' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			hyperparameterCap.topAgentCount = engine.teamSize;
			if ((e = hyperparameterCapElement->QueryUnsigned64Attribute("MutationCount", &hyperparameterCap.mutationCount)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.mutationCount' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.mutationCount'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("BiasMutationRate", &hyperparameterCap.biasMutationRate)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.biasMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.biasMutationRate'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("BiasMutationMagnitude", &hyperparameterCap.biasMutationMagnitude)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.biasMutationMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.biasMutationMagnitude'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("WeightMutationRate", &hyperparameterCap.weightMutationRate)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.weightMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.weightMutationRate'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("WeightMutationMagnitude", &hyperparameterCap.weightMutationMagnitude)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.weightMutationMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.weightMutationMagnitude'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("SynapseMutationRate", &hyperparameterCap.synapseMutationRate)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.synapseMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.synapseMutationRate'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("NewSynapseMagnitude", &hyperparameterCap.newSynapseMagnitude)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.newSynapseMagnitude' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.newSynapseMagnitude'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("NodeMutationRate", &hyperparameterCap.nodeMutationRate)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.nodeMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.nodeMutationRate'" << std::endl;
			if ((e = hyperparameterCapElement->QueryFloatAttribute("LayerMutationRate", &hyperparameterCap.layerMutationRate)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.layerMutationRate' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.layerMutationRate'" << std::endl;
			if ((e = hyperparameterCapElement->QueryUnsigned64Attribute("NewLayerSizeAverage", &hyperparameterCap.newLayerSizeAverage)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.newLayerSizeAverage' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.newLayerSizeAverage'" << std::endl;
			if ((e = hyperparameterCapElement->QueryUnsigned64Attribute("NewLayerSizeRange", &hyperparameterCap.newLayerSizeRange)) != tinyxml2::XML_SUCCESS) std::cerr << "ERROR: Failed to load config Attribute 'hyperparameterCap.newLayerSizeRange' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl; else std::cerr << "INFO: Loaded config Attribute 'hyperparameterCap.newLayerSizeRange'" << std::endl;
		}
		else
		{
			std::cerr << "ERROR: 'HyperparameterCap' element not found in the configuration file." << std::endl;
		}
	}


	/// <summary>
	/// Save the configs to file
	/// </summary>
	void Save()
	{
		tinyxml2::XMLError e;
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement* root = doc.NewElement("Config");
		doc.InsertFirstChild(root);

		// Save App settings
		tinyxml2::XMLElement* appElement = doc.NewElement("App");
		appElement->SetAttribute("screenWidth", app.screenWidth);
		appElement->SetAttribute("screenHeight", app.screenHeight);
		appElement->SetAttribute("targetFPS", app.targetFPS);
		root->InsertEndChild(appElement);

		// Save UI settings
		tinyxml2::XMLElement* uiElement = doc.NewElement("UI");

		// Save primaryColor
		tinyxml2::XMLElement* backgroundColorElement = doc.NewElement("BackgroundColor");
		backgroundColorElement->SetAttribute("r", ui.backgroundColor.r);
		backgroundColorElement->SetAttribute("g", ui.backgroundColor.g);
		backgroundColorElement->SetAttribute("b", ui.backgroundColor.b);
		backgroundColorElement->SetAttribute("a", ui.backgroundColor.a);
		uiElement->InsertEndChild(backgroundColorElement);

		// Save primaryColor
		tinyxml2::XMLElement* primaryColorElement = doc.NewElement("PrimaryColor");
		primaryColorElement->SetAttribute("r", ui.primaryColor.r);
		primaryColorElement->SetAttribute("g", ui.primaryColor.g);
		primaryColorElement->SetAttribute("b", ui.primaryColor.b);
		primaryColorElement->SetAttribute("a", ui.primaryColor.a);
		uiElement->InsertEndChild(primaryColorElement);

		// Save secondaryColor
		tinyxml2::XMLElement* secondaryColorElement = doc.NewElement("SecondaryColor");
		secondaryColorElement->SetAttribute("r", ui.secondaryColor.r);
		secondaryColorElement->SetAttribute("g", ui.secondaryColor.g);
		secondaryColorElement->SetAttribute("b", ui.secondaryColor.b);
		secondaryColorElement->SetAttribute("a", ui.secondaryColor.a);
		uiElement->InsertEndChild(secondaryColorElement);

		// Save textColor
		tinyxml2::XMLElement* textColorElement = doc.NewElement("TextColor");
		textColorElement->SetAttribute("r", ui.textColor.r);
		textColorElement->SetAttribute("g", ui.textColor.g);
		textColorElement->SetAttribute("b", ui.textColor.b);
		textColorElement->SetAttribute("a", ui.textColor.a);
		uiElement->InsertEndChild(textColorElement);

		// Save baseTextSize
		uiElement->SetAttribute("FPSTextSize", ui.fpsTextSize);
		root->InsertEndChild(uiElement);

		// Save filePaths
		tinyxml2::XMLElement* filePathsElement = doc.NewElement("FilePaths");
		filePathsElement->SetAttribute("ModelFolder", filePaths.modelFolder.string().c_str());
		root->InsertEndChild(filePathsElement);

		// Save engine
		tinyxml2::XMLElement* engineElement = doc.NewElement("Engine");
		engineElement->SetAttribute("SizeX", engine.sizeX);
		engineElement->SetAttribute("SizeY", engine.sizeY);
		engineElement->SetAttribute("TeamSize", engine.teamSize);
		engineElement->SetAttribute("AgentBaseHealth", engine.agentBaseHealth);
		engineElement->SetAttribute("UpdateDelta", engine.updateDelta);
		engineElement->SetAttribute("ResetTime", engine.resetTime);
		root->InsertEndChild(engineElement);

		// Save hyperparameterCap
		tinyxml2::XMLElement* hyperparameterCapElement = doc.NewElement("HyperparameterCap");
		//hyperparameterCapElement->SetAttribute("TopAgentCount", hyperparameterCap.topAgentCount);
		hyperparameterCapElement->SetAttribute("MutationCount", hyperparameterCap.mutationCount);
		hyperparameterCapElement->SetAttribute("BiasMutationRate", hyperparameterCap.biasMutationRate);
		hyperparameterCapElement->SetAttribute("BiasMutationMagnitude", hyperparameterCap.biasMutationMagnitude);
		hyperparameterCapElement->SetAttribute("WeightMutationRate", hyperparameterCap.weightMutationRate);
		hyperparameterCapElement->SetAttribute("WeightMutationMagnitude", hyperparameterCap.weightMutationMagnitude);
		hyperparameterCapElement->SetAttribute("SynapseMutationRate", hyperparameterCap.synapseMutationRate);
		hyperparameterCapElement->SetAttribute("NewSynapseMagnitude", hyperparameterCap.newSynapseMagnitude);
		hyperparameterCapElement->SetAttribute("NodeMutationRate", hyperparameterCap.nodeMutationRate);
		hyperparameterCapElement->SetAttribute("LayerMutationRate", hyperparameterCap.layerMutationRate);
		hyperparameterCapElement->SetAttribute("NewLayerSizeAverage", hyperparameterCap.newLayerSizeAverage);
		hyperparameterCapElement->SetAttribute("NewLayerSizeRange", hyperparameterCap.newLayerSizeRange);
		root->InsertEndChild(hyperparameterCapElement);

		// Save to file
		if ((e = doc.SaveFile(filePaths.configPath.string().c_str())) == tinyxml2::XML_SUCCESS)
		{
			std::cerr << "INFO: Config file saved successfully" << std::endl;
		}
		else
		{
			std::cerr << "ERROR: saving config file '" << filePaths.configPath << "'TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
		}
	}
};