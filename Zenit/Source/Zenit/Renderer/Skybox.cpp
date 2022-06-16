#include "Skybox.h"
#include "Zenit/Core/Log.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Shader.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Zenit {

	Skybox::Skybox(std::vector<std::string> faces)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                ZN_CORE_ERROR("Skybox texture failed at {0}", faces[i]);
                stbi_image_free(data);
                //glDeleteTextures(1, &id);
                //return;
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        float vertices[] =
        {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };


        vao = std::make_shared<VertexArray>();
        vbo = std::make_shared<VertexBuffer>(vertices, 108);
        vbo->SetLayout({ {ShaderDataType::VEC3F, "position"} });
        vao->AddVertexBuffer(vbo.get());

        shader = new Shader("Assets/Shaders/skybox.shader");

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        intensity = 1.0f;
        enableReflection = true;

        ZN_CORE_INFO("Skybox loaded successfully");
	}

	Skybox::~Skybox()
	{
        glDeleteTextures(1, &id);
	}

    void Skybox::Bind(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    void Skybox::Unbind()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Skybox::Draw()
    {
        //glDepthFunc(GL_LEQUAL);
        //shader->Bind();
        //shader->SetUniformMatrix4f("view", glm::mat3(camera.GetView()));
        //shader->SetUniformMatrix4f("projection", camera.GetProjection());
        //
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        //shader->SetUniform1i("skybox", 0);
        
        
        vao->Bind();
        vbo->Bind();
        glDrawArrays(GL_TRIANGLES, 0, vbo->GetCount() / 3);
        vao->Unbind();
        vbo->Unbind();
        
        
        glDepthFunc(GL_LESS);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        shader->Unbind();
    }

}