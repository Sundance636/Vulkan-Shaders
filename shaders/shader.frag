#version 450

//fragment shader is ran once per pixel (in a given triangle),
layout (location = 0) in vec3 fragColor; //specifying as flat induces flat shading model
layout(location = 1) in float waveDx;
layout(location = 2) in float waveDz;
layout(location = 3) in vec3 eyeline;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
  float deltaTime;
  mat4 modelMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(14.0f,-15.0f,9.0f));//light source
const float AMBIENCE = 0.01f;


void main() {
  vec3 tangentX = vec3(1.0, waveDx, 0.0);
  vec3 tangentZ = vec3(0.0, waveDz, 1.0);

  vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * cross(tangentX, tangentZ));


  vec3 reflectionVec = normalize(reflect(-DIRECTION_TO_LIGHT,normalWorldSpace));
  vec3 halfanglevec = -DIRECTION_TO_LIGHT +eyeline;

  //later seperate light source from cam position
  float specularMagnitude = max(0.0f, dot( normalize(halfanglevec),normalWorldSpace));
  specularMagnitude = pow(specularMagnitude,64);

  //specular need the direction of the light source and the view direction

  //ambient light + diffuse light
  float DIFFUSE = AMBIENCE + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT),0);
  DIFFUSE = DIFFUSE + specularMagnitude;

  vec3 color = vec3(0.2f,0.3f,0.8f);

  outColor = vec4(DIFFUSE * color, 1.0);
}