struct Particle {
  float3 position;
  float _pad1;
  float3 velocity;
  float _pad2;
};

struct WorldData {
  float4x4 view;
  float4x4 model;
  float4x4 proj;
  float3 cameraPos;
  float _pad1;
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
