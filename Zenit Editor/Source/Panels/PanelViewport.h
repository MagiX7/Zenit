#pragma once

#include "Zenit.h"

#include <memory>

namespace Zenit {

	class PanelViewport
	{
	public:
		PanelViewport();
		~PanelViewport();

		void OnUpdate(TimeStep ts, Model* model);
		void OnImGuiRender(FrameBuffer* fbo, const PerspectiveCamera& camera);

	private:
		glm::vec2 viewportSize;
		bool allowModelRotation;
	};

}