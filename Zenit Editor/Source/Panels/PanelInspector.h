#pragma once


namespace Zenit {

	class Model;
	struct DirectionalLight;

	class PanelInspector
	{
	public:
		void OnImGuiRender(Model* model, DirectionalLight& light);
	};
}