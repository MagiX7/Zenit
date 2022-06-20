#include "Zenit/Core/Resources/Model.h"

#include "PanelInspector.h"

#include "EditorLayer.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void PanelInspector::OnImGuiRender(Model* model, DirectionalLight& light)
	{
		ImGui::Begin("Inspector");

		if (ImGui::CollapsingHeader("Light"))
		{
			ImGui::Indent();

			ImGui::DragFloat3("Direction", glm::value_ptr(light.dir));
			ImGui::ColorPicker3("Ambient", glm::value_ptr(light.ambient));
			ImGui::ColorPicker3("Diffuse", glm::value_ptr(light.diffuse));

			ImGui::Unindent();
		}

		ImGui::Separator();

		if (model && ImGui::Button("Reset Rotation"))
		{
			model->ResetRotation();
		}

		ImGui::End();
	}
}
