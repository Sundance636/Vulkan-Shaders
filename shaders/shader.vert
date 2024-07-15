#version 450

#define M_PI 3.1415926535897932384626433832795

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
const float AMBIENCE = 0.09f;

float rand(vec2 co);
vec3 sumofsines(vec3 position, float time);

// Simple hash function
float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

// 2D hash function
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Function to generate nearby random values based on vertex position
vec3 generateRandomValues(vec3 position) {
    float randomX = hash(position.x + position.y * 57.0);
    float randomY = hash(position.y + position.z * 57.0);
    float randomZ = hash(position.z + position.x * 57.0);
    return vec3(randomX, randomY, randomZ);
}

void main() {
  //gl_Position = vec4( push.transform * position , 0.0, 1.0);
  float Amplitude = position.y/10;
  float frequency = 3;
  float waveLength = 1.5;

  int sum = 6;
  float AMPLITUDE = position.y;//rand
  float FREQUENCY = 3;//rand
  float WAVELENGTH = 1.5;
  vec3 DIRECTION = vec3(1,0,1);//rand
  vec3 DIRECTION_SUM = vec3(0,0,0);//rand
  float SPEED = 1;//5.0f * (2* M_PI/WAVELENGTH);

  vec3 waves  = vec3(position.x,0,position.z);

  for(int i = 0; i < sum; i++) {

   AMPLITUDE = position.y/10 * hash(i) ;//rand
   FREQUENCY = 3  * 1+hash(i);//rand
   WAVELENGTH = 1.5 * 1+hash(i);
   DIRECTION = vec3(1* hash(i+1) * hash(i+1),1* hash(i+2),1* hash(i+3)) ;// + vec3(rand(vec2(position.x, position.y)), 0, rand(vec2(position.x, position.y)));//rand
   DIRECTION_SUM += DIRECTION;
   SPEED = 2;//5.0f * (2* M_PI/WAVELENGTH);

    waves.y += AMPLITUDE * sin((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime * SPEED)/WAVELENGTH);
  
  }


  vec3 wavePos = vec3(position.x, Amplitude * sin((frequency * position.x + ubo.deltaTime)/waveLength) ,position.z);
  //wavePos = sumofsines(position,ubo.deltaTime);
  gl_Position = ubo.projectionViewMatrix  * vec4(waves,1.0f);
  //gl_Position = ubo.projectionViewMatrix * vec4(position,1.0f);
  

  //need to recalculate normals before using them for lighting

  float waveDx = AMPLITUDE * cos((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime* SPEED) / WAVELENGTH) * FREQUENCY / WAVELENGTH;
  float waveDz = AMPLITUDE * cos((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime* SPEED) / WAVELENGTH) * FREQUENCY / WAVELENGTH;;

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


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}