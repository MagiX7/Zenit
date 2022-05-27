#include "Zenit/Renderer/Skybox.h"

#include "PanelSkybox.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void Zenit::PanelSkybox::OnImGuiRender(const std::unique_ptr<Skybox>& skybox)
	{
		ImGui::Begin("Skybox Settings");
		
		ImGui::Checkbox("Enable Reflection", &skybox->IsReflectionEnabled());
		if(skybox->IsReflectionEnabled())
			ImGui::DragFloat("Intensity", &skybox->GetInstensity(), 0.05, 0);

		ImGui::End();
	}

}
