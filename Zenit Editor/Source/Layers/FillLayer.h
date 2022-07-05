#pragma once

#include "Layer.h"

namespace Zenit {

	class FillLayer : public Layer
	{
	public:
		FillLayer(int id);
		virtual void OnImGuiRender() override;

	};

}
