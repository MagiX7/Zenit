#pragma once

#include "Zenit.h"
#include "Panels/PanelInspector.h"

namespace Zenit {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

	private:
		PanelInspector panelInspector;
	};


}