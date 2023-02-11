#include "Entity.h"

namespace Zenit {

	Entity::Entity()
	{
		shader = std::make_shared<Shader>("Assets/Shaders/pbr.shader");
		
		uint32_t data = 0xffffffff;
		diffuse = std::make_shared<Texture2D>(&data, 1, 1);
		diffuse->SetName("white");
		normals = std::make_shared<Texture2D>(&data, 1, 1);
		normals->SetName("white");
		metallic = std::make_shared<Texture2D>(&data, 1, 1);
		metallic->SetName("white");
		roughness = std::make_shared<Texture2D>(&data, 1, 1);
		roughness->SetName("white");
	}

	Entity::~Entity()
	{
	}

}