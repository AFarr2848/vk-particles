struct Particle {
  float3 position;
  float density;
  float3 velocity;
  uint32_t cellHash;
};

struct WorldData {
  float4x4 view;
  float4x4 model;
  float4x4 proj;
  float3 cameraPos;

  Particle* particleBufferAddress;
  uint32_t* particleLookupBufferAddress;

  float3 particleWorldSize;

  // Particle params
  float aspect;
  float particleSize;
  float smoothRadius;
  float densityDrawConst;
  float targetDensity;
  float pressureMultiplier;
  float gravity;
  float collisionDampingConst;
  float drag;
  float mass;
};

struct PushConstants {
  WorldData* worldDataBufferAddress;
  float deltaTime;
  uint particleCount;
  uint data1;
  uint data2;
};

struct VSOutput {
  float4 pos : SV_Position;
  float4 color : COLOR;
  float2 uv : TEXCOORD0;
};

static const float PI = 3.14159265f;
static const float E = 2.71828182f;

