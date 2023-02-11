#pragma once

#include "Zenit/Renderer/Shader.h"
#include "Zenit/Renderer/Texture2D.h"

#include <memory>

namespace Zenit {

	class Entity
	{
	public:
		Entity();
		virtual ~Entity();

		void SetDiffuse(Texture2D* texture) { diffuse.reset(texture); };
		void SetNormals(Texture2D* texture) { normals.reset(texture); };
		void SetMetallic(Texture2D* texture) { metallic.reset(texture); };
		void SetRoughness(Texture2D* texture) { roughness.reset(texture); };

	protected:
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Texture2D> diffuse;
		std::shared_ptr<Texture2D> normals;
		std::shared_ptr<Texture2D> metallic;
		std::shared_ptr<Texture2D> roughness;
	};

}