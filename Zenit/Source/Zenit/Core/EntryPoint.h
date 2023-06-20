#pragma once

#include "Application.h"

extern Zenit::Application* Zenit::CreateApplication();

#ifdef ZN_RELEASE
	#include <Windows.h>
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
	int main()
#endif
	{
		Zenit::Application* app = Zenit::CreateApplication();
		app->Run();
		delete app;
	}
