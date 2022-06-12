#pragma once
#define GLM_FORCE_RADIANS

#include "LearningVulkanApplication.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <engine/renderer/drawables/primitives/cube.hpp>

namespace ExampleApp
{
    class ExampleApp : public LearningVulkan::LearningVulkanApplication
    {
    public:
        ExampleApp(std::string appName) : LearningVulkan::LearningVulkanApplication(appName) {}

        void onCreate() override
        {
            const std::vector<engine::Vertex> vertices = {
                {glm::vec3(-1.0f, -1.0f, 0.0f),
                 glm::vec2(0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, -1.0f, 0.0f),
                 glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.0f, .0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(1.0f, 1.0f, 0.0f),
                 glm::vec2(1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)},
                {glm::vec3(-1.0f, 1.0f, 0.0f),
                 glm::vec2(0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 0.0f)}};

            std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0};

            myObjectId = RegisterMesh(vertices, indices, false);

            auto cube_data = m_pCube.data();
            CubeId = RegisterMesh(cube_data.Vertices, cube_data.Indices, false);
            {
                int w, h, nc;
                unsigned char *pixels = framework::load_image_from_file("../assets/images/texture.png", &w, &h, &nc);
                engine::TextureInfo texInfo;
                texInfo.width = w;
                texInfo.height = h;
                texInfo.numChannels = nc;
                auto texture = m_pRenderer->RegisterTexture(pixels, texInfo);

                material = m_pRenderer->CreateMaterial(texture);
            }
/*
            {
                int w, h, nc;
                unsigned char *pixels = framework::load_image_from_file("untitled.png", &w, &h, &nc);
                engine::TextureInfo texInfo;
                texInfo.width = w;
                texInfo.height = h;
                texInfo.numChannels = nc;
                auto texture = m_pRenderer->RegisterTexture(pixels, texInfo);

                material2 = m_pRenderer->CreateMaterial(texture);
            }*/
        }

        void onRender() override
        {
            engine::ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            camData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            camData.proj = glm::perspective(glm::radians(45.0f), getWindowDimensions().first / (float)getWindowDimensions().second, 0.1f, 10.0f);
            camData.proj[1][1] *= -1;
            camData.iTime += time;

            PushCameraData(camData);

            myObjectId->push_constant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            engine::RenderObject renderObject;
            renderObject.objectId = myObjectId;
            renderObject.materialId = material.id;
            renderObject.uniformData = camData;

            Draw(renderObject);

            CubeId->push_constant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(.0f, .0f, 1.0f));
            CubeId->push_constant.model = glm::scale(CubeId->push_constant.model, glm::vec3(0.5));

            renderObject.objectId = CubeId;
            renderObject.materialId = material.id;
            renderObject.uniformData = camData;
            Draw(renderObject);
        }

        void onExit() override
        {
        }

    private:
        engine::Renderer::ObjectDataId myObjectId = {};
        engine::Material material;
        engine::Material material2;

        engine::Cube m_pCube;
        engine::Renderer::ObjectDataId CubeId = {};
    };
}