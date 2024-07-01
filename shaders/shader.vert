#version 450


//runs once for each vertex in the vertex buffer
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

//layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat3 transform;
  vec2 offset;
  vec3 color;
} push;

void main() {
  gl_Position = vec4( push.transform * position , 1.0);
  //fragColor = color;

}