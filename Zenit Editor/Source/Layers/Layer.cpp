#include "Layer.h"

namespace Zenit {

	Layer::Layer(LayerType layerType, int id) : type(layerType), id(id)
	{
		uint32_t data = 0xffffffff;
		texture = std::make_unique<Texture2D>(&data, 1, 1);
		name = "Layer " + std::to_string(id);
	}
}
