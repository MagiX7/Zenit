#pragma once

#include "Zenit.h"

#include <string>
#include <memory>

namespace Zenit {

	enum class LayerType
	{
		NONE = -1,
		FILL,
		PAINT
	};

	class Layer
	{
	public:
		Layer(LayerType layerType, int id);

		virtual void OnImGuiRender() = 0;

		std::string& GetName() { return name; }
		void SetName(const std::string& newName) { name = newName; }

		const std::unique_ptr<Texture2D>& GetTexture() { return texture; }

	private:
		LayerType type;
		std::string name;
		std::unique_ptr<Texture2D> texture;
		int id;
	};

}