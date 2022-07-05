#include "PanelLayerStack.h"
#include "Layers/FillLayer.h"

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Zenit {

	void PanelLayerStack::OnImGuiRender(std::stack<Layer*>& layers) const
	{
		ImGui::Begin("Layers");

		static bool showCreationMenu = false;
		static int layerId = 0;
		if (ImGui::Button("+"))
		{
			showCreationMenu = true;
		}

		if (showCreationMenu)
		{
			if (ImGui::MenuItem("Fill Layer"))
			{
				layers.push(new FillLayer(layerId++));
				showCreationMenu = false;
			}
			else if (ImGui::MenuItem("Paint Layer"))
			{
				showCreationMenu = false;
			}
		}

		std::stack<Layer*> stack = layers;
		while (!stack.empty())
		{
			Layer* layer = stack.top();

			ImGui::Spacing();
			ImGui::SetNextItemWidth(150.0f);
			ImGui::InputText("Name", &layer->GetName());
			ImGui::Image((void*)layer->GetTexture()->GetId(), { 64,64 });
			ImGui::Spacing();
			ImGui::Separator();

			stack.pop();
		}

		ImGui::End();
	}
}
