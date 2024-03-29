#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace engine {

    struct Vertex
    {
        Vertex() = default;

        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n): position(p), texCoords(uv), normal(n) {}
        
        Vertex(glm::vec3 p, glm::vec2 uv, glm::vec3 n, glm::vec4 c, glm::vec3 t, glm::vec3 bt) : position(p), texCoords(uv), normal(n), color(c), tangent(t), bitangent(bt)
        {
        }


        alignas(32) glm::vec3 position = glm::vec3(0.0f);
        alignas(32) glm::vec2 texCoords = glm::vec2(0.0f);
        alignas(32) glm::vec3 normal = glm::vec3(0.0f);
        alignas(32) glm::vec4 color = glm::vec4(0.0f);
        alignas(32) glm::vec3 tangent = glm::vec3(0.0f);
        alignas(32) glm::vec3 bitangent = glm::vec3(0.0f);
    };
}