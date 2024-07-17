#pragma once
#include <iostream>
#include <string>
#include "raylib.h"
#include "tinyxml2.h"

static std::string MakeFilename(const std::string& filename, const std::string& extension) {
	std::string correctExtension = (extension[0] == '.') ? extension : "." + extension;
	size_t lastDot = filename.find_last_of('.');
	std::string baseFilename = (lastDot != std::string::npos) ? filename.substr(0, lastDot) : filename;
	return baseFilename + correctExtension;
}

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
		std::string configPath;
		std::string modelFolder = "models/";
	};
	FilePaths filePaths;
	struct Engine
	{
		float sizeX;
		float sizeY;
	};
	Engine engine;

	Config(std::string configPath) : filePaths({configPath,""}) 
	{
		Load();
	}


	void Load()
	{

		tinyxml2::XMLError e;
		tinyxml2::XMLDocument doc;
		{
			if ((e = doc.LoadFile(filePaths.configPath.c_str())) != tinyxml2::XML_SUCCESS)
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
			tinyxml2::XMLElement* backgroundColorElement = uiElement->FirstChildElement("BackgroundColor");
			if (backgroundColorElement)
			{
				int intValue;

				if ((e = backgroundColorElement->QueryIntAttribute("r", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.backgroundColor.r = static_cast<unsigned char>(intValue);
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.backgroundColor.r' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = backgroundColorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.backgroundColor.g = static_cast<unsigned char>(intValue);
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.backgroundColor.g' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}
				
				if ((e = backgroundColorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.backgroundColor.b = static_cast<unsigned char>(intValue);
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.backgroundColor.b' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = backgroundColorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.backgroundColor.a = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.backgroundColor.a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}
			}
			else
			{
				std::cerr << "Error: 'UI.BackgroundColor' element not found in the configuration file." << std::endl;
			}

			// Loading primaryColor
			tinyxml2::XMLElement* primaryColorElement = uiElement->FirstChildElement("PrimaryColor");
			if (primaryColorElement)
			{
				int intValue; // Temporary integer variable for attribute retrieval

				if ((e = primaryColorElement->QueryIntAttribute("r", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.primaryColor.r = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.primaryColor.r' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = primaryColorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.primaryColor.g = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.primaryColor.g' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = primaryColorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.primaryColor.b = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.primaryColor.b' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = primaryColorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.primaryColor.a = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.primaryColor.a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}
			}
			else
			{
				std::cerr << "Error: 'UI.PrimaryColor' element not found in the configuration file." << std::endl;
			}

			// Loading secondaryColor
			tinyxml2::XMLElement* secondaryColorElement = uiElement->FirstChildElement("SecondaryColor");
			if (secondaryColorElement)
			{
				int intValue; // Temporary integer variable for attribute retrieval

				if ((e = secondaryColorElement->QueryIntAttribute("r", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.secondaryColor.r = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.secondaryColor.r' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = secondaryColorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.secondaryColor.g = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.secondaryColor.g' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = secondaryColorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.secondaryColor.b = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.secondaryColor.b' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = secondaryColorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.secondaryColor.a = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.secondaryColor.a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}
			}
			else
			{
				std::cerr << "Error: 'UI.SecondaryColor' element not found in the configuration file." << std::endl;
			}

			// Loading textColor
			tinyxml2::XMLElement* textColorElement = uiElement->FirstChildElement("TextColor");
			if (textColorElement)
			{
				int intValue; // Temporary integer variable for attribute retrieval

				if ((e = textColorElement->QueryIntAttribute("r", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.textColor.r = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.textColor.r' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = textColorElement->QueryIntAttribute("g", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.textColor.g = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.textColor.g' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = textColorElement->QueryIntAttribute("b", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.textColor.b = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.textColor.b' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}

				if ((e = textColorElement->QueryIntAttribute("a", &intValue)) == tinyxml2::XML_SUCCESS)
				{
					ui.textColor.a = static_cast<unsigned char>(intValue); // Convert int to unsigned char
				}
				else
				{
					std::cerr << "Error: Failed to load config Attribute 'ui.textColor.a' TinyXMLError[" << e << "] = " << tinyxml2::XMLDocument::ErrorIDToName(e) << std::endl;
				}
			}
			else
			{
				std::cerr << "Error: 'UI.TextColor' element not found in the configuration file." << std::endl;
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
		}
		else
		{
			std::cerr << "Error: 'Engine' element not found in the configuration file." << std::endl;
		}

	}

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

		// Save FilePaths
		tinyxml2::XMLElement* filePathsElement = doc.NewElement("FilePaths");
		filePathsElement->SetAttribute("ModelFolder", filePaths.modelFolder.c_str());
		root->InsertEndChild(filePathsElement);

		// Save FilePaths
		tinyxml2::XMLElement* engineElement = doc.NewElement("Engine");
		engineElement->SetAttribute("SizeX", engine.sizeX);
		engineElement->SetAttribute("SizeY", engine.sizeY);
		root->InsertEndChild(engineElement);

		// Save to file
		if (doc.SaveFile(filePaths.configPath.c_str()) == tinyxml2::XML_SUCCESS)
		{
			std::cout << "Config file saved successfully." << std::endl;
		}
		else
		{
			std::cerr << "Error saving config file: " << filePaths.configPath << std::endl;
		}
	}
};