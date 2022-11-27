#include "Zenit/Renderer/FrameBuffer.h"
#include "Zenit/Core/Log.h"

#include "PanelViewport.h"

#include <ImGui/imgui.h>

#define ROTATION_MULTIPLIER 200

namespace Zenit {

	PanelViewport::PanelViewport()
	{
		viewportSize = glm::vec2(0);
		allowModelRotation = false;
	}

	PanelViewport::~PanelViewport()
	{
	}

	void PanelViewport::OnUpdate(TimeStep ts, Model* model, PerspectiveCamera& camera)
	{
		if (!allowModelRotation || !model)
			return;

		camera.Scroll(ts);

		float x = Input::GetInstance()->GetMouseX();
		float y = Input::GetInstance()->GetMouseY();

		const float dx = Input::GetInstance()->GetMouseMotionX();
		const float dy = Input::GetInstance()->GetMouseMotionY();
		
		if (Input::GetInstance()->IsMouseButtonPressed(MOUSE_LEFT))
			model->Update(ts, dx * ROTATION_MULTIPLIER, dy * ROTATION_MULTIPLIER);
	}

	void PanelViewport::OnImGuiRender(FrameBuffer* fbo, PerspectiveCamera& camera)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
		ImGui::Begin("Viewport");

		ImGui::IsWindowHovered() ? allowModelRotation = true : allowModelRotation = false;
		
		ImVec2 dimensions = ImGui::GetContentRegionAvail();
		if (viewportSize.x != dimensions.x || viewportSize.y != dimensions.y)
		{
			fbo->Resize(dimensions.x, dimensions.y);
			Renderer3D::OnResize(dimensions.x, dimensions.y);
			camera.SetApsectRatio(dimensions.x / dimensions.y);
			ZN_TRACE("Viewport Resized");
			viewportSize = { dimensions.x, dimensions.y };
		}
		
		ImGui::Image((ImTextureID*)fbo->GetColorId(), { viewportSize.x, viewportSize.y }, { 0,1 }, { 1,0 });
		ImGui::End();
		ImGui::PopStyleVar();
	}

}