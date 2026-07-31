#include "include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

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
  float3 worldPos = p.position + float3(cornerOffset, 0.0);
  output.pos = mul(pc.worldDataBufferAddress.proj, float4(worldPos, 1.0));
  output.color = float4(p.velocity, 1.0);
  return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  return input.color;
}
