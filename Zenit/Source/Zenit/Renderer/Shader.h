#pragma once

#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/vec2.hpp"

#include <unordered_map>
#include <string>

namespace Zenit {

	enum class ShaderType
	{
		FRAGMENT = 0,
		VERTEX,
		GEOMETRY
	};

	struct UniformData
	{
		unsigned int type;
		float data;
		std::string name;
	};

	class Shader
	{
	public:
		Shader(const std::string& path);
		virtual ~Shader();

		void Bind();
		void Unbind();

		bool Refresh();

		inline const std::string& GetName() { return name; }
		inline void SetName(const std::string n) { name = n; }
		char* GetLastModifiedDate();

		void SetUniformBool(const std::string& name, bool b);

		void SetUniform1i(const std::string& name, int i);
		void SetUniform1f(const std::string& name, float f);

		void SetUnifromVec2f(const std::string& name, float v0, float v1);
		void SetUnifromVec2f(const std::string& name, glm::vec2 v);

		void SetUniformVec3f(const std::string& name, float v0, float v1, float v2);
		void SetUniformVec3f(const std::string& name, glm::vec3 v);

		void SetUniformVec4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformVec4f(const std::string& name, glm::vec4 v);

		void SetUniformMatrix3f(const std::string& name, const glm::mat3x3& mat);
		void SetUniformMatrix4f(const std::string& name, const glm::mat4x4& mat);

		inline const std::string& GetPath() { return path; }

		std::list<UniformData> GetUniforms();
		void UpdateSourceCode(const std::string& newSource);

	private:
		unsigned int CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
		std::string ReadFile();
		std::unordered_map<unsigned int, std::string> SplitShaders(const std::string& source);

		void ReCompile(bool reReadSource = true);

	private:
		std::string name;
		unsigned int rendererID;
		std::string source;
		std::string path;

		bool created;

		std::unordered_map<std::string, int> uniformsLocation;

		struct _stat lastStat;
		char timeBuf[26];
	};

}