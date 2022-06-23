#include "Zenit/Renderer/FrameBuffer.h"
#include "Zenit/Core/Log.h"

#include "PanelViewport.h"


#include <ImGui/imgui.h>

namespace Zenit {

	PanelViewport::PanelViewport()
	{
		viewportSize = glm::vec2(0);
		allowModelRotation = false;
	}

	PanelViewport::~PanelViewport()
	{
	}

	void PanelViewport::OnUpdate(TimeStep ts, Model* model)
	{
		if (!allowModelRotation)
			return;

		float x = Input::GetInstance()->GetMouseX();
		float y = Input::GetInstance()->GetMouseY();

		const float dx = Input::GetInstance()->GetMouseMotionX();
		const float dy = Input::GetInstance()->GetMouseMotionY();
		
		if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_RIGHT))
			model->Update(ts, dx, dy);
	}

	void PanelViewport::OnImGuiRender(FrameBuffer* fbo, const PerspectiveCamera& camera)
	{
		ImGui::Begin("Viewport");

		ImGui::IsWindowHovered() ? allowModelRotation = true : allowModelRotation = false;
		
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