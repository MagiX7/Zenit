#pragma once

#include <memory>

namespace Zenit {

	class Skybox;

	class PanelSkybox
	{
	public:
		//PanelSkybox();
		//~PanelSkybox();

		void OnImGuiRender(const std::unique_ptr<Skybox>& skybox, bool& drawSkybox);
	};

}