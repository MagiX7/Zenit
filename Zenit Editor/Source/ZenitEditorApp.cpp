#include "Zenit.h"
#include "Zenit/Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Zenit {

	class ZenitEditorApp : public Application
	{
	public:
		ZenitEditorApp()
		{
			PushLayer(new EditorLayer());

			//ModelImporter::ImportModel("Assets/Models/Gun/Gun.dae");

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