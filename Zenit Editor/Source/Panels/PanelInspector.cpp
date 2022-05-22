#include "Zenit/Core/Resources/Model.h"

#include "PanelInspector.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void PanelInspector::OnImGuiRender(Model* model)
	{
		ImGui::Begin("Inspector");
		if (model && ImGui::Button("Reset Rotation"))
		{
			model->ResetRotation();
		}

		ImGui::End();
	}
}