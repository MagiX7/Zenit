#include "Skybox.h"
#include "Zenit/Core/Log.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "FrameBuffer.h"
#include "Shader.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Zenit {

	Skybox::Skybox(std::vector<std::string> faces)
	{
		glGenTextures(1, &cubemapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

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

        skyboxShader = new Shader("Assets/Shaders/skybox.shader");

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        intensity = 1.0f;
        enableReflection = false;

        ZN_CORE_INFO("Skybox loaded successfully");
	}

    Skybox::Skybox(std::string path)
    {
        SetUpCube();

        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
        
        if (!data)
        {
            ZN_CORE_ERROR("Failed to load HDR image");
            return;
        }
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);


        // Generate Cubemap texture with empty data to later fill it
        glGenTextures(1, &cubemapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        // Convert HDR image to Cubemap
        hdrToCubemapShader = std::make_shared<Shader>("Assets/shaders/hdr_to_cubemap.shader");
        hdrToCubemapShader->Bind();
        hdrToCubemapShader->SetUniformMatrix4f("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        hdrToCubemapShader->SetUniform1i("hdrMap", 0);


        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glViewport(0, 0, 512, 512);

        for (int i = 0; i < 6; ++i)
        {
            hdrToCubemapShader->SetUniformMatrix4f("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapID, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Draw();
        }
        hdrToCubemapShader->Unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //glGenTextures(1, &irradianceMapID);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
        //for (unsigned int i = 0; i < 6; ++i)
        //{
        //    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
        //}
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //
        //glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        //glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

        //irradianceMapShader = std::make_shared<Shader>("Assets/Shaders/irradiance_skybox.shader");
        //
        //
        //irradianceMapShader->Bind();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
        //irradianceMapShader->SetUniform1i("environmentMap", 0);
        //irradianceMapShader->SetUniformMatrix4f("projection", captureProjection);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
        //
        //glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
        //glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        //for (unsigned int i = 0; i < 6; ++i)
        //{
        //    irradianceMapShader->SetUniformMatrix4f("view", captureViews[i]);
        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        //        irradianceMapID, 0);
        //    
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //    Draw();
        //}
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);



        //glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        intensity = 1.0f;
        enableReflection = false;

        ZN_CORE_INFO("Skybox with HDR texture loaded successfully");

    }

	Skybox::~Skybox()
	{
        glDeleteTextures(1, &cubemapID);
	}

    void Skybox::Bind(int slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
    }

    void Skybox::Unbind()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Skybox::Draw()
    {       
        vao->Bind();
        vbo->Bind();
        glDrawArrays(GL_TRIANGLES, 0, vbo->GetCount());
        vao->Unbind();
        vbo->Unbind();
    }

    void Skybox::BindPreComputedData()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMapID);
    }

    void Skybox::SetUpCube()
    {
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
    }
}