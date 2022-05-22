#pragma once

#include "Zenit.h"

#include <memory>

namespace Zenit {

	class PanelViewport
	{
	public:
		PanelViewport();
		~PanelViewport();

		void OnImGuiRender(FrameBuffer* fbo, PerspectiveCamera camera, Model* model);

	private:
		glm::vec2 viewportSize;

	};

}