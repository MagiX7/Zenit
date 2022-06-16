#pragma once

#include <memory>

namespace Zenit {

	class Skybox;
	struct SkyboxProperties;

	class PanelSkybox
	{
	public:

		void OnImGuiRender(const std::unique_ptr<Skybox>& skybox, SkyboxProperties& props);
	};

}