#pragma once

#include "Zenit/Core/PerspectiveCamera.h"

#include <memory>
#include <vector>
#include <string>

namespace Zenit {

	class VertexBuffer;
	class VertexArray;
	class Shader;

	class Skybox
	{
	public:
		Skybox(std::vector<std::string> faces);
		~Skybox();

		void Bind(int slot = 0);
		void Unbind();

		void Draw();

		inline unsigned int GetId() { return id; }

		inline float& GetInstensity() { return intensity; }
		inline float SetInstensity(float value) { intensity = value; }

		inline bool& IsReflectionEnabled() { return enableReflection; }
		inline void SetIsReflectionEnabled(bool value) { enableReflection = value; }


	private:
		unsigned int id;
		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<VertexArray> vao;
		Shader* shader;
		
		float intensity;
		bool enableReflection;
	};

}