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
		Skybox(std::string path);
		~Skybox();

		void Draw(const glm::mat3& view, const glm::mat4& projection);

		inline const std::string& GetName() { return name; }
		inline const std::string& GetPath() { return path; }
		inline unsigned int GetId() { return cubemapID; }
		inline unsigned int GetIrradianceMapId() { return irradianceMapID; }
		inline unsigned int GetHDRId() { return hdrTexture; }
		inline unsigned int GetPrefilterMapId() { return prefilterMap; }
		inline unsigned int GetBRDFId() { return brdfTexture; }
		inline unsigned int GetFBO() { return captureFBO; }
		inline unsigned int GetRBO() { return captureRBO; }

		inline float& GetReflectionLod() { return reflectionLod; }
		inline float SetReflectionLod(float value) { reflectionLod = value; }

		inline bool& IsReflectionEnabled() { return enableReflection; }
		inline void SetIsReflectionEnabled(bool value) { enableReflection = value; }

		void BindIrradianceMap(int slot);
		void BindPrefilterMap(int slot);
		void BindBRDF(int slot);

	private:
		void SetUpCube();
		void SetUpQuad();
		
		void DrawCube();
		void DrawQuad();

	private:
		std::string name;
		std::string path;
		unsigned int captureFBO, captureRBO;
		unsigned int cubemapID, irradianceMapID;
		unsigned int hdrTexture, brdfTexture;
		unsigned int prefilterMap;

		std::unique_ptr<VertexBuffer> cubeVbo;
		std::unique_ptr<VertexArray> cubeVao;
		std::unique_ptr<VertexBuffer> quadVbo;
		std::unique_ptr<VertexArray> quadVao;
		std::unique_ptr<Shader> skyboxShader;
		std::unique_ptr<Shader> hdrToCubemapShader;
		std::unique_ptr<Shader> irradianceMapShader;
		std::unique_ptr<Shader> prefilterShader;
		std::unique_ptr<Shader> brdfShader;
		
		float reflectionLod;
		bool enableReflection;
	};

}