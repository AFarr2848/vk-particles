struct Particle {
  float3 position;
  float _pad1;
  float3 velocity;
  float _pad2;
};

struct PushConstants {
  Particle* particleBufferAddress;
  float deltaTime;
  uint particleCount;
};

[[vk::push_constant]]
PushConstants pc;

struct VSOutput {
  float4 pos : SV_Position;
  float4 color : COLOR;
  float2 uv : TEXCOORD0;
};

static const float2 QUAD_OFFSETS[4] = { float2(-0.5, -0.5), float2(0.5, -0.5), float2(-0.5, 0.5),
                                        float2(0.5, 0.5) };

static const float2 QUAD_UVS[4] = { float2(0.0, 0.0), float2(1.0, 0.0), float2(0.0, 1.0),
                                    float2(1.0, 1.0) };

[shader("vertex")]
VSOutput vertexMain(uint vertexID: SV_VertexID, uint instanceID: SV_InstanceID) {
  VSOutput output;

  Particle p = pc.particleBufferAddress[instanceID];

  float2 cornerOffset = QUAD_OFFSETS[vertexID] * 0.01;
  output.uv = QUAD_UVS[vertexID];
  output.color = float4(p.position, 1.0f);

  float4 worldPos = float4(p.position, 1.0);
  worldPos.xy += cornerOffset;
  worldPos.z = 0;
  output.pos = worldPos;
  output.color = float4(p.velocity, 1.0);
  return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return input.color;
}
