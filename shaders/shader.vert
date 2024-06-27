#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
/*
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;*/

vec2 positions[3] = vec2[] (
    vec2(0.0f,-0.5f),
    vec2(0.5f,0.5f),
    vec2(-0.5f,-0.5f)
);

void main() {
    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
    fragColor = inColor;
}