#version 450

//fragment shader is ran once per pixel (in a given triangle),
layout (location = 0) in vec3 fragColor;
layout (location = 0) out vec4 outColor;

void main() {
  outColor = vec4(fragColor, 1.0);
}