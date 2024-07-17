#include <iostream>
#include "Application.h"
#include "Configs.h"
int main()
{
	Config config("config.xml");
	config.Save();
	Application app (config);
	app.Run();
}
