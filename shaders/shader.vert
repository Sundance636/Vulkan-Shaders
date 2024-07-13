#version 450


//runs once for each vertex in the vertex buffer
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 directionToLight;
  float deltaTime;

} ubo;

layout(push_constant) uniform Push {
  float deltaTime;
  mat4 modelMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0f,-3.0f,-1.0f));
const float AMBIENCE = 0.02f;

void main() {
  //gl_Position = vec4( push.transform * position , 0.0, 1.0);
  float Amplitude = position.y;
  float frequency = 2;
  float waveLength = 1.5;

  vec3 wavePos = vec3(position.x, Amplitude * sin((position.x + ubo.deltaTime)/waveLength) ,position.z);
  gl_Position = ubo.projectionViewMatrix  * vec4(wavePos,1.0f);

  vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);

  float lightIntensity = AMBIENCE + max(dot(normalWorldSpace, normalize(ubo.directionToLight - position)),0);

  fragColor = lightIntensity * color;

}