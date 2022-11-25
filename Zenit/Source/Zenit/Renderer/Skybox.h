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

		void Draw(const glm::mat3& view, const glm::mat4& projection);

		inline unsigned int GetId() { return cubemapID; }
		inline unsigned int GetIrradianceMapId() { return irradianceMapID; }
		inline unsigned int GetHDRId() { return hdrTexture; }
		inline unsigned int GetPrefilterMapId() { return prefilterMap; }

		inline float& GetInstensity() { return intensity; }
		inline float SetInstensity(float value) { intensity = value; }

		inline bool& IsReflectionEnabled() { return enableReflection; }
		inline void SetIsReflectionEnabled(bool value) { enableReflection = value; }

		void BindIrradianceMap(int slot);

	private:
		void SetUpCube();
		void DrawCube();

	private:
		unsigned int captureFBO, captureRBO;
		unsigned int cubemapID, irradianceMapID;
		unsigned int hdrTexture;
		unsigned int prefilterMap;

		std::unique_ptr<VertexBuffer> vbo;
		std::unique_ptr<VertexArray> vao;
		std::unique_ptr<Shader> skyboxShader;
		std::unique_ptr<Shader> hdrToCubemapShader;
		std::unique_ptr<Shader> irradianceMapShader;
		std::unique_ptr<Shader> prefilterShader;
		
		float intensity;
		bool enableReflection;
	};

}