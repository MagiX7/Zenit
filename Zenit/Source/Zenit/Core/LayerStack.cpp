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
	void LayerStack::PushLayer(Layer* layer)
	{
		layers.emplace(layers.begin() + layerIndex, layer);
		layer->OnAttach();
		layerIndex++;
	}

	// Overlays to stay at the end of the vector
	void LayerStack::PushOverlay(Layer* overlay)
	{
		layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		std::vector<Layer*>::iterator it = std::find(layers.begin(), layers.end(), layer);
		if (it != layers.end())
		{
			layers.erase(it);
			layerIndex--;
		}

		layer->OnDetach();
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		std::vector<Layer*>::iterator it = std::find(layers.begin(), layers.end(), overlay);
		if (it != layers.end())
			layers.erase(it);

		overlay->OnDetach();
	}

}
