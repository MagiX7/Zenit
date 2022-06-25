#include "Zenit.h"
#include "Zenit/Core/EntryPoint.h"

#include "EditorLayer.h"

using namespace Assimp;

namespace Zenit {

	class ZenitEditorApp : public Application
	{
	public:
		ZenitEditorApp()
		{
			PushLayer(new EditorLayer());
		}

		~ZenitEditorApp()
		{

		}

	};

	Application* CreateApplication()
	{
		return new ZenitEditorApp();
	}
}