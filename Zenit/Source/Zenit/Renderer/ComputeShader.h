#pragma once

#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/vec2.hpp"

#include <string>
#include <unordered_map>

namespace Zenit {

	class ComputeShader
	{
	public:
		ComputeShader(const std::string& path);
		virtual ~ComputeShader();

		void Bind() const;
		void Unbind() const;

		void SetUniformBool(const std::string& name, bool b);

		void SetUniform1i(const std::string& name, int i);
		void SetUniform1f(const std::string& name, float f);

		void SetUniformVec2f(const std::string& name, float v0, float v1);
		void SetUniformVec2f(const std::string& name, glm::vec2 v);

		void SetUniformVec3f(const std::string& name, float v0, float v1, float v2);
		void SetUniformVec3f(const std::string& name, glm::vec3 v);

		void SetUniformVec4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformVec4f(const std::string& name, glm::vec4 v);

		void SetUniformMatrix3f(const std::string& name, const glm::mat3x3& mat);
		void SetUniformMatrix4f(const std::string& name, const glm::mat4x4& mat);

	private:
		unsigned int CreateShader();
		std::string ReadFile();

	private:
		std::string name;
		unsigned int rendererID;
		std::string source;
		std::string path;

		int workGroupCount[3];
		int workGroupSize[3];

	};

}
