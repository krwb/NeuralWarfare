#include <iostream>
#include "Application.h"
#include "Configs.h"
int main()
{
	//I'd put unit tests here if i had time

	//Load the configs
	Config config("config.xml");
	config.Save();

	//Start the app
	Application app (config);
	app.Run();
}
