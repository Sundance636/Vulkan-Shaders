#version 450

//fragment shader is ran once per pixel (in a given triangle),
layout (location = 0) in vec3 fragColor; //specifying as flat induces flat shading model

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
  float deltaTime;
  mat4 modelMatrix;
} push;

void main() {
  outColor = vec4(fragColor, 1.0);
}