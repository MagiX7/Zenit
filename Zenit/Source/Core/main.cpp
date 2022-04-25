#include <iostream>

#include "Application.h"


int main()
{
	Zenit::Application* app = new Zenit::Application();
	app->Run();
	delete app;

	return 0;
}
