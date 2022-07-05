#pragma once

#include "EngineLayer.h"
#include <vector>

namespace Zenit {

	class LayerStack
	{
	public:
		LayerStack();
		virtual ~LayerStack();

		void PushLayer(EngineLayer* layer);
		void PushOverlay(EngineLayer* overlay);

		void PopLayer(EngineLayer* layer);
		void PopOverlay(EngineLayer* overlay);

		std::vector<EngineLayer*>::iterator begin() { return layers.begin(); }
		std::vector<EngineLayer*>::iterator end() { return layers.end(); }

	private:
		std::vector<EngineLayer*> layers;
		unsigned int layerIndex = 0;

	};

}