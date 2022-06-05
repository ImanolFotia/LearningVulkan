#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace engine
{
    enum renderer_type
    {
        vulkan = 0,
        dx12,
        dx11,
        opengl,
        metal,
        size // if we ever want to iterate over renderer types
    };

    struct Vertex
    {
        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt) : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt)
        {
        }

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texCoords = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec4 color = glm::vec4(0.0f);
        glm::vec3 tangent = glm::vec3(0.0f);
        glm::vec3 bitangent = glm::vec3(0.0f);
    };

    struct Mesh
    {

        void addTriangle(uint32_t a, uint32_t b, uint32_t c)
        {
            Indices.push_back(a);
            Indices.push_back(b);
            Indices.push_back(c);
        }

        void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
        {
            Indices.push_back(a);
            Indices.push_back(b);
            Indices.push_back(d);
            Indices.push_back(b);
            Indices.push_back(c);
            Indices.push_back(d);
        }

        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };

    struct ShaderData
    {
        alignas(4) float iTime;
        alignas(8) glm::vec2 iResolution;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };
}