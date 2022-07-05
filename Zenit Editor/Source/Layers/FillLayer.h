#pragma once

#include "Layer.h"

namespace Zenit {

	class FillLayer : public Layer
	{
	public:
		FillLayer();
		virtual void OnImGuiRender() override;

	};

}
