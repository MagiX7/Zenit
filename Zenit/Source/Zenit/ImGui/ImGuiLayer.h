#pragma once

#include "Zenit/Core/EngineLayer.h"

#include "Zenit/Events/ApplicationEvent.h"
#include "Zenit/Events/Event.h"
#include "Zenit/Events/KeyEvent.h"
#include "Zenit/Events/MouseEvent.h"


namespace Zenit {

	class ImGuiLayer : public EngineLayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

	private:
		void SetStyle();

	private:
		float time = 0;
	};


}