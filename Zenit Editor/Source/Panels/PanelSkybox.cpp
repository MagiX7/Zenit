#include "Zenit/Renderer/Skybox.h"

#include "PanelSkybox.h"
#include "Helpers/SkyboxProperties.h"

#include <ImGui/imgui.h>

namespace Zenit {

	void Zenit::PanelSkybox::OnImGuiRender(const std::unique_ptr<Skybox>& skybox, SkyboxProperties& props)
	{
		ImGui::Begin("Skybox Settings");

		ImGui::Checkbox("Draw Skybox", &props.draw);

		if (props.draw)
		{
			ImGui::Checkbox("Enable Reflection", &skybox->IsReflectionEnabled());
			if (skybox->IsReflectionEnabled())
			{
				ImGui::DragFloat("Intensity", &skybox->GetInstensity(), 0.05f, 0);
			}

			ImGui::Checkbox("Enable blur", &props.blurEnabled);
			if (props.blurEnabled)
			{
				ImGui::DragFloat("Blur amount", &props.blurAmount);
			}
		}

		ImGui::End();
	}

}
