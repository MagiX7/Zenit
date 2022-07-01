#pragma once

namespace Zenit {

	class Model;
	struct DirectionalLight;

	struct Node;

	class PanelInspector
	{
	public:
		void OnImGuiRender(Model* model, DirectionalLight& light, Node* currentNode);

	private:
		void ShowNodeSpecs(Node* node);
	};
}