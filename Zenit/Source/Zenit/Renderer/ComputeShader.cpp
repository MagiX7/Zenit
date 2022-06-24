#include "ComputeShader.h"

#include "Zenit/Core/Log.h"

#include <glad/glad.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Zenit {

	ComputeShader::ComputeShader(const std::string& path) : path(path)
	{
		const int s = path.find_last_of("/");
		const int e = path.find_last_of(".");
		name = path.substr(s + 1, e - s - 1);

		source = ReadFile();
		CreateShader();
	}

	ComputeShader::~ComputeShader()
	{
		glDeleteProgram(rendererID);
	}

	void ComputeShader::Bind() const
	{
		glUseProgram(rendererID);
	}

	void ComputeShader::Unbind() const
	{
		glUseProgram(0);
	}

	unsigned int ComputeShader::CreateShader()
	{
		GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			ZN_CORE_ERROR("{0} Compute Shader compilation failed {1}", name, infoLog);
		}

		rendererID = glCreateProgram();
		glAttachShader(rendererID, shader);
		glLinkProgram(rendererID);
		glValidateProgram(rendererID);

		glGetProgramiv(rendererID, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetProgramInfoLog(rendererID, 512, NULL, infoLog);
			ZN_CORE_ERROR("{0} Compute Shader linking failed {1}", name, infoLog);
		}
		else
		{
			ZN_CORE_INFO("{0} Compute Shader linked successfully!", name);

			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

			ZN_CORE_INFO("    Max texture size supported: {1} {2} {3}", workGroupSize[0], workGroupSize[1], workGroupSize[2]);

		}

		glDetachShader(rendererID, shader);
		glDeleteShader(shader);

		return 0;
	}

	void ComputeShader::SetUniformBool(const std::string& name, bool b)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform1f(location, b);
	}

	void ComputeShader::SetUniform1i(const std::string& name, int i)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform1i(location, i);
	}

	void ComputeShader::SetUniform1f(const std::string& name, float f)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform1f(location, f);
	}

	void ComputeShader::SetUniformVec2f(const std::string& name, float v0, float v1)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform2f(location, v0, v1);
	}

	void ComputeShader::SetUniformVec2f(const std::string& name, glm::vec2 v)
	{
		SetUniformVec2f(name, v.x, v.y);
	}

	void ComputeShader::SetUniformVec3f(const std::string& name, float v0, float v1, float v2)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform3f(location, v0, v1, v2);
	}

	void ComputeShader::SetUniformVec3f(const std::string& name, glm::vec3 v)
	{
		SetUniformVec3f(name, v.x, v.y, v.z);
	}

	void ComputeShader::SetUniformVec4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniform4f(location, v0, v1, v2, v3);
	}

	void ComputeShader::SetUniformVec4f(const std::string& name, glm::vec4 v)
	{
		SetUniformVec4f(name, v.x, v.y, v.z, v.w);
	}

	void ComputeShader::SetUniformMatrix3f(const std::string& name, const glm::mat3x3& mat)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ComputeShader::SetUniformMatrix4f(const std::string& name, const glm::mat4x4& mat)
	{
		GLint location = glGetUniformLocation(rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}

	std::string ComputeShader::ReadFile()
	{
		std::string ret;
		std::ifstream in(path, std::ios::in, std::ios::binary);

		if (in)
		{
			// We seek for the end of file so we can set the returning string size
			in.seekg(0, std::ios::end);
			ret.resize(in.tellg());

			// We now seek for the file beginning to put it at the string (passing the string's first position and its size)
			in.seekg(0, std::ios::beg);
			in.read(&ret[0], ret.size());

			// Finally close the file
			in.close();
		}
		else
		{
			ZN_CORE_ERROR("{0}: Could not locate the Compute Shader", name);
		}

		return ret;
	}

}
