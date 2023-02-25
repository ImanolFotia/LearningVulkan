#version 450

const vec2 iResolution = vec2(1280.0, 720.0);

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout (location = 0) out vec3 position;
layout (location = 1) out vec2 texCoords;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec4 color;
layout (location = 4) out int piece;

layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
    vec4 lastMove;
} ubo;

layout( push_constant ) uniform constants
{
	mat4 model;
	int piece;
} PushConstants;

void main() {
    color = inColor;
    piece = PushConstants.piece;
    position = inPosition;
    texCoords = inTexCoord;
    normal = inNormal; 


    gl_Position = ubo.proj * ubo.view * PushConstants.model  * vec4(inPosition, 1.0);
}
