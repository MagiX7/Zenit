#pragma once

#include "Application.h"

extern Zenit::Application* Zenit::CreateApplication();

int main()
{
	Zenit::Application* app = Zenit::CreateApplication();
	app->Run();
	delete app;	
}