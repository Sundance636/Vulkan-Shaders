#version 450

#define M_PI 3.1415926535897932384626433832795
#define EULER 2.71828182845904

//runs once for each vertex in the vertex buffer
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 UV;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out float waveDx;
layout(location = 2) out float waveDz;
layout(location = 3) out vec3 eyeline;


layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 eye;//view direction
  float deltaTime;

} ubo;

layout(push_constant) uniform Push {
  float deltaTime;
  mat4 modelMatrix;
} push;

float rand(vec2 co);
vec3 generateRandomValues(vec3 position);
float hash(vec2 p);
float hash(float n);



void main() {
  //gl_Position = vec4( push.transform * position , 0.0, 1.0);
  float Amplitude = position.y/1;
  float frequency = 3;
  float waveLength = 1.5;
  eyeline = ubo.eye;

  int sum = 9;
  float AMPLITUDE = position.y;//rand
  float FREQUENCY = 3;//rand
  float WAVELENGTH = 1.5;
  vec3 DIRECTION = vec3(1,0,1);//rand
  vec3 DIRECTION_SUM = vec3(0,0,0);//rand
  float SPEED = 1;//5.0f * (2* M_PI/WAVELENGTH);

  vec3 waves  = vec3(position.x,0,position.z);
  waveDx =0.0f;
  waveDz = 0.0f;

  for(int i = 0; i < sum; i++) {

   AMPLITUDE = position.y/15 * hash(i) ;//rand
   FREQUENCY = 3.5  * 1+hash(i);//rand
   WAVELENGTH = 1.5 * 1+hash(i);
   DIRECTION = normalize(vec3(1* hash(i+1) * hash(i+1),1* hash(i+2),1* hash(i+3))) ;// + vec3(rand(vec2(position.x, position.y)), 0, rand(vec2(position.x, position.y)));//rand
   DIRECTION_SUM += DIRECTION;
   SPEED = 4;//5.0f * (2* M_PI/WAVELENGTH);

   waves.y += AMPLITUDE * sin((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime * SPEED)/WAVELENGTH);
    
    waveDx += AMPLITUDE * cos((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime* SPEED) / WAVELENGTH) * FREQUENCY / WAVELENGTH;
    waveDz += AMPLITUDE * cos((FREQUENCY * dot(DIRECTION, position) + ubo.deltaTime* SPEED) / WAVELENGTH) * FREQUENCY / WAVELENGTH;;

  }


  vec3 wavePos = vec3(position.x, Amplitude * sin((frequency * position.x + ubo.deltaTime)/waveLength) ,position.z);
  gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(waves,1.0f);
  //gl_Position = ubo.projectionViewMatrix * vec4(position,1.0f);
  

  //need to recalculate normals before using them for lighting



}

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

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}