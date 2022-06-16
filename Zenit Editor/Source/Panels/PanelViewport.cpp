#include "Zenit/Renderer/FrameBuffer.h"
#include "Zenit/Core/Log.h"

#include "PanelViewport.h"

#include <ImGui/imgui.h>

namespace Zenit {

	PanelViewport::PanelViewport()
	{
		viewportSize = glm::vec2(0);
	}

	PanelViewport::~PanelViewport()
	{
	}

	void PanelViewport::OnImGuiRender(FrameBuffer* fbo, const PerspectiveCamera& camera)
	{
		ImGui::Begin("Viewport");
		ImVec2 dimensions = ImGui::GetContentRegionAvail();
		if (viewportSize.x != dimensions.x || viewportSize.y != dimensions.y)
		{
			fbo->Resize(dimensions.x, dimensions.y);
			Renderer3D::OnResize(dimensions.x, dimensions.y);
			//camera.UpdateFov(dimensions.x, dimensions.y);
			
			ZN_CORE_TRACE("Viewport Resized");
			viewportSize = { dimensions.x, dimensions.y };
		}
		
		ImGui::Image((ImTextureID*)fbo->GetColorId(), { viewportSize.x, viewportSize.y }, { 0,1 }, { 1,0 });
		ImGui::End();
	}

}