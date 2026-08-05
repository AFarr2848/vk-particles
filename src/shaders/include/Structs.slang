struct Particle {
  float3 position;
  float value1;
  float3 velocity;
  float value2;
};

struct WorldData {
  float4x4 view;
  float4x4 model;
  float4x4 proj;
  float3 cameraPos;
  // Particle params
  float aspect;
  float particleSize;
  float smoothRadius;
  float densityDrawConst;
};

struct PushConstants {
  Particle* particleBufferAddress;
  WorldData* worldDataBufferAddress;
  float deltaTime;
  uint particleCount;
};

struct VSOutput {
  float4 pos : SV_Position;
  float4 color : COLOR;
  float2 uv : TEXCOORD0;
};

static const float PI = 3.14159265f;
