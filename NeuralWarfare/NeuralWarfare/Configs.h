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
		Color primaryColor = LIGHTGRAY;
		Color secondaryColor = GRAY;
		Color textColor = BLACK;
		float baseTextSize = 20;
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
	};
	Engine engine;

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
				std::cerr << "Error: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->g = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "Error: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->b = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "Error: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			if ((e = colorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
			{
				value->a = static_cast<unsigned char>(intValue);
			}
			else
			{
				std::cerr << "Error: Failed to load config color Attribute '" << element->Name() << "." << name << ".a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}
		}
		else
		{
			std::cerr << "Error: Color Element '" << element->Name() << "." << name << "'  not found in the configuration file." << std::endl;
		}
		return e;
	}

	/// <summary>
	/// load the configs from file
	/// </summary>
	void Load()
	{

		tinyxml2::XMLError e;
		tinyxml2::XMLDocument doc;
		{
			if ((e = doc.LoadFile(filePaths.configPath.string().c_str())) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "Error loading XML file: " << filePaths.configPath << "TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				return;
			}
		}

		tinyxml2::XMLElement* root = doc.RootElement();
		if (!root) 
		{
			std::cerr << "Error: No root element found in XML file." << std::endl;
			return;
		}

		tinyxml2::XMLElement* appElement = root->FirstChildElement("App");
		if (appElement) 
		{
			if ((e = appElement->QueryIntAttribute("screenWidth", &app.screenWidth)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'app.screenWidth' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			if ((e = appElement->QueryIntAttribute("screenHeight", &app.screenHeight)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'app.screenHeight' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			if ((e = appElement->QueryIntAttribute("targetFPS", &app.targetFPS)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'app.targetFPS' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
		}
		else
		{
			std::cerr << "Error: 'App' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* uiElement = root->FirstChildElement("UI");
		if (uiElement) 
		{
			// Loading backgroundColor
			if ((e = LoadColor(uiElement,"BackgroundColor",&ui.backgroundColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "Error: Failed to load color 'ui.backgroundColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			// Loading primaryColor
			if ((e = LoadColor(uiElement, "PrimaryColor", &ui.primaryColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "Error: Failed to load color 'ui.primaryColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			// Loading secondaryColor
			if ((e = LoadColor(uiElement, "SecondaryColor", &ui.secondaryColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "Error: Failed to load color 'ui.secondaryColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			// Loading textColor
			if ((e = LoadColor(uiElement, "TextColor", &ui.textColor)) != tinyxml2::XML_SUCCESS)
			{
				std::cerr << "Error: Failed to load color 'ui.textColor' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			}

			// Loading baseTextSize
			if ((e = uiElement->QueryFloatAttribute("BaseTextSize", &ui.baseTextSize)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'ui.baseTextSize' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;

		}
		else
		{
			std::cerr << "Error: 'UI' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* filePathsElement = root->FirstChildElement("FilePaths");
		if (filePathsElement)
		{
			filePaths.modelFolder = filePathsElement->Attribute("ModelFolder");
		}
		else
		{
			std::cerr << "Error: 'FilePaths' element not found in the configuration file." << std::endl;
		}

		tinyxml2::XMLElement* engineElement = root->FirstChildElement("Engine");
		if (engineElement)
		{
			if ((e = engineElement->QueryFloatAttribute("SizeX", &engine.sizeX)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'engine.sizeX' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			if ((e = engineElement->QueryFloatAttribute("SizeY", &engine.sizeY)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'engine.sizeY' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			if ((e = engineElement->QueryUnsigned64Attribute("TeamSize", &engine.teamSize)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'engine.teamSize' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
			if ((e = engineElement->QueryFloatAttribute("AgentBaseHealth", &engine.agentBaseHealth)) != tinyxml2::XML_SUCCESS) std::cerr << "Error: Failed to load config Attribute 'engine.agentBaseHealth' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;

		}
		else
		{
			std::cerr << "Error: 'Engine' element not found in the configuration file." << std::endl;
		}

	}

	/// <summary>
	/// Save the configs to file
	/// </summary>
	void Save()
	{
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
		uiElement->SetAttribute("BaseTextSize", ui.baseTextSize);
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
		root->InsertEndChild(engineElement);

		// Save to file
		if (doc.SaveFile(filePaths.configPath.string().c_str()) == tinyxml2::XML_SUCCESS)
		{
			std::cout << "Config file saved successfully." << std::endl;
		}
		else
		{
			std::cerr << "Error saving config file: " << filePaths.configPath << std::endl;
		}
	}
};