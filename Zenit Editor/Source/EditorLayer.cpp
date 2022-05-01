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
	}

	void EditorLayer::OnDetach()
	{
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Viewport");
		ImGui::End();

		panelInspector.OnImGuiRender();
	}
}