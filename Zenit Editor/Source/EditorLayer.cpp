#include "EditorLayer.h"

#include <ImGui/imgui.h>

namespace Zenit {

	EditorLayer::EditorLayer()
	{
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		fbo = std::make_unique<FrameBuffer>(1280, 720, 0);
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool showDemoWindow = false;
		ImGui::BeginMainMenuBar();
		if (ImGui::MenuItem("File"))
		{

		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Show Demo Window"))
				showDemoWindow = !showDemoWindow;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		ImGui::Begin("Viewport");
		ImGui::Image((void*)fbo->GetColorId(), { (float)fbo->GetWidth(), (float)fbo->GetHeight() }, { 0,1 }, { 1,0 });
		ImGui::End();

		panelInspector.OnImGuiRender();

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

	}
}