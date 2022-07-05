#include "LayerStack.h"

namespace Zenit {

	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		for (auto& l : layers)
			delete l;
	}

	// Layers to stay at the beginning of the vector
	void LayerStack::PushLayer(EngineLayer* layer)
	{
		layers.emplace(layers.begin() + layerIndex, layer);
		layer->OnAttach();
		layerIndex++;
	}

	// Overlays to stay at the end of the vector
	void LayerStack::PushOverlay(EngineLayer* overlay)
	{
		layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(EngineLayer* layer)
	{
		std::vector<EngineLayer*>::iterator it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layers.erase(it);
			layerIndex--;
		}

		layer->OnDetach();
	}

	void LayerStack::PopOverlay(EngineLayer* overlay)
	{
		std::vector<EngineLayer*>::iterator it = std::find(layers.begin(), layers.end(), overlay);
		if (it != layers.end())
			layers.erase(it);

		overlay->OnDetach();
	}

}
