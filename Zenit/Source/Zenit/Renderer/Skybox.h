#pragma once

#include "Zenit/Core/PerspectiveCamera.h"

#include <memory>
#include <vector>
#include <string>

namespace Zenit {

	class VertexBuffer;
	class VertexArray;
	class FrameBuffer;
	class Shader;

	class Skybox
	{
	public:
		Skybox(std::vector<std::string> faces);
		Skybox(std::string path);
		~Skybox();

		void Bind(int slot = 0);
		void Unbind();

		void Draw();

		inline unsigned int GetId() { return cubemapID; }
		inline unsigned int GetIrradianceMapId() { return irradianceMapID; }
		inline unsigned int GetHDRId() { return hdrTexture; }

		inline float& GetInstensity() { return intensity; }
		inline float SetInstensity(float value) { intensity = value; }

		inline bool& IsReflectionEnabled() { return enableReflection; }
		inline void SetIsReflectionEnabled(bool value) { enableReflection = value; }

		void BindIrradianceMap(int slot);

	private:
		void SetUpCube();

	private:
		unsigned int captureFBO, captureRBO;
		unsigned int cubemapID, irradianceMapID;
		unsigned int hdrTexture;

		std::shared_ptr<VertexBuffer> vbo;
		std::shared_ptr<VertexArray> vao;
		Shader* skyboxShader;
		std::shared_ptr<Shader> hdrToCubemapShader;
		std::shared_ptr<Shader> irradianceMapShader;
		
		float intensity;
		bool enableReflection;
	};

}