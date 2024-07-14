#version 450


//runs once for each vertex in the vertex buffer
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 directionToLight;//view direction
  float deltaTime;

} ubo;

layout(push_constant) uniform Push {
  float deltaTime;
  mat4 modelMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(50.0f,-100.0f,-10.0f));//light source
const float AMBIENCE = 0.02f;

void main() {
  //gl_Position = vec4( push.transform * position , 0.0, 1.0);
  float Amplitude = position.y/10;
  float frequency = 3;
  float waveLength = 1.5;


  vec3 wavePos = vec3(position.x, Amplitude * sin((frequency * position.x + ubo.deltaTime)/waveLength) ,position.z);
  gl_Position = ubo.projectionViewMatrix  * vec4(wavePos,1.0f);
  //gl_Position = ubo.projectionViewMatrix * vec4(position,1.0f);
  
  //need to recalculate normals before using them for lighting

  float waveDx = Amplitude * cos((frequency * position.x + ubo.deltaTime) / waveLength) * frequency / waveLength;
  float waveDz = 0.0;

  vec3 tangentX = vec3(1.0, waveDx, 0.0);
  vec3 tangentZ = vec3(0.0, waveDz, 1.0);

  vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * cross(tangentZ, tangentX));

  vec3 reflectionVec = normalize(reflect(-DIRECTION_TO_LIGHT,normalWorldSpace));
  //later seperate light source from cam position
  float specularMagnitude = max(0.0f, dot( normalize(ubo.directionToLight),reflectionVec));
  specularMagnitude = pow(specularMagnitude,64);

  //specular need the direction of the light source and the view direction

  //ambient light + diffuse light
  float lightIntensity = AMBIENCE + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT),0);
  lightIntensity = lightIntensity + specularMagnitude;
  fragColor = lightIntensity * color;
    fragColor = lightIntensity * vec3(0.2f,0.3f,0.8f);


}