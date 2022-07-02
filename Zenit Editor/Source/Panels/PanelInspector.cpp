#include "Zenit/Core/Resources/Model.h"

#include "EditorLayer.h"
#include "PanelInspector.h"

#include "Helpers/Nodes/Node.h"
#include "Helpers/Nodes/ColorNode.h"
#include "Helpers/Nodes/PerlinNoiseNode.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void PanelInspector::OnImGuiRender(Model* model, DirectionalLight& light, Node* currentNode)
	{
		ImGui::Begin("Inspector");

		if (ImGui::CollapsingHeader("Light"))
		{
			ImGui::Indent();
			ImGui::Text("Direction");
			ImGui::SameLine();
			ImGui::DragFloat3("###Direction", glm::value_ptr(light.dir));
			ImGui::Separator();

			ImGui::SetNextItemWidth(200);
			ImGui::ColorPicker3("Ambient", glm::value_ptr(light.ambient));
			ImGui::SetNextItemWidth(200);
			ImGui::ColorPicker3("Diffuse", glm::value_ptr(light.diffuse));

			ImGui::Unindent();
		}

		ImGui::Separator();

		if (model && ImGui::Button("Reset Rotation"))
		{
			model->ResetRotation();
		}

		ShowNodeSpecs(currentNode);

		ImGui::End();
	}

	void PanelInspector::ShowNodeSpecs(Node* node)
	{
		if (!node)
			return;

		switch(node->type)
		{
			case NodeType::PERLIN_NOISE:
			{
				ImGui::Separator();
				const auto n = (PerlinNoiseNode*)node;
				ImGui::DragFloat("Seed", &n->seed);
				ImGui::Image((void*)n->texture->GetId(), { 300,300 });
				break;
			}

			case NodeType::COLOR:
			{
				const auto n = (ColorNode*)node;
				ImGui::SetNextItemWidth(300);
				ImGui::ColorPicker3("Color", glm::value_ptr(n->color));
				break;
			}

			default: break;
		}

	}
}
