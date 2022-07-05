#pragma once

#include "../Layers/Layer.h"
#include <stack>

namespace Zenit {

	class PanelLayerStack
	{
	public:
		void OnImGuiRender(std::stack<Layer*>& layers) const;
	};

}
