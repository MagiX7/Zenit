#include "Zenit/Core/Resources/Model.h"

#include "EditorLayer.h"
#include "PanelInspector.h"

#include "Nodes/Node.h"
#include "Nodes/ColorNode.h"
#include "Nodes/NoiseNode.h"
#include "Nodes/VoronoiNode.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void PanelInspector::OnImGuiRender(Model* model, DirectionalLight& light, Node* currentNode)
	{
		ImGui::Begin("Inspector");

		if (model && ImGui::Button("Reset Rotation"))
		{
			model->ResetRotation();
		}

		if (currentNode)
			currentNode->OnImGuiInspectorRender();

		ImGui::End();
	}
}
