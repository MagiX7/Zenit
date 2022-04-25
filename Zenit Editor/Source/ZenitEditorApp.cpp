#include "Zenit.h"

class ZenitEditorApp : public Zenit::Application
{
public:
	ZenitEditorApp()
	{

	}

	~ZenitEditorApp()
	{

	}
};

Zenit::Application* Zenit::CreateApplication()
{
	return new ZenitEditorApp();
}