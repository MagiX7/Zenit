#include "Layer.h"

namespace Zenit {

	Layer::Layer(const std::string& name, bool enabled) : name(name), enabled(enabled)
	{
	}

	Layer::~Layer()
	{
	}
}