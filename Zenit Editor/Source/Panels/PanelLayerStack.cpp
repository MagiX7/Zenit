#include "PanelLayerStack.h"
#include "Layers/FillLayer.h"

#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>

namespace Zenit {

	void PanelLayerStack::OnImGuiRender(std::stack<Layer*>& layers) const
	{
		ImGui::Begin("Layers");

		if (ImGui::BeginMenu("+"))
		{
			if (ImGui::MenuItem("Fill Layer"))
			{
				layers.push(new FillLayer());
			}
			else if (ImGui::MenuItem("Paint Layer"))
			{
				
			}
			ImGui::EndMenu();
		}

		std::stack<Layer*> stack = layers;
		while (!stack.empty())
		{
			Layer* layer = stack.top();

			//ImGui::InputText("Name", &layer->GetName());
			ImGui::NewLine();
			ImGui::Image((void*)layer->GetTexture()->GetId(), { 20,20 });

			stack.pop();
		}

		ImGui::End();
	}
}
