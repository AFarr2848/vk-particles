#include "include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

static const float2 QUAD_OFFSETS[4] = { float2(-1., -1.), float2(1., -1.), float2(-1., 1.),
                                        float2(1., 1.) };

static const float2 QUAD_UVS[4] = { float2(0.0, 0.0), float2(1.0, 0.0), float2(0.0, 1.0),
                                    float2(1.0, 1.0) };

[shader("vertex")]
VSOutput vertexMain(uint vertexID: SV_VertexID, uint instanceID: SV_InstanceID) {
  VSOutput output;

  Particle p = pc.worldDataBufferAddress.particleBufferAddress[instanceID];
  float size = pc.worldDataBufferAddress.particleSize;

  float2 cornerOffset = QUAD_OFFSETS[vertexID] * size;
  output.uv = QUAD_UVS[vertexID];
  float3 worldPos = p.position + float3(cornerOffset, 0.0);
  output.pos = mul(pc.worldDataBufferAddress.proj, float4(worldPos, 1.0));
  output.color = float4(length(p.velocity) * 3, 1 - length(p.velocity) * 3,  0, 1);
  return output;
}

float smoothingKernel(float dist) {
  const float smoothRadius = pc.worldDataBufferAddress.smoothRadius;
  float volume = float.getPi() * pow(smoothRadius, 8) / 4;
  float value = max(0, smoothRadius * smoothRadius - dist * dist);
  return value * value * value / volume;
}

float calculateDensity(float3 p) {
  float density = 0;
  float mass = 1;
  for (int i = 0; i < pc.particleCount; i++) {
    float dist = length(pc.worldDataBufferAddress.particleBufferAddress.position - p);
    float influence = smoothingKernel(dist);
    density += mass * influence;
  }
  return density;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  float size = pc.worldDataBufferAddress.particleSize;
  float len = length(input.uv - float2(0.5));
  if (len > 0.5)
    discard;
  // return float4(float3(calculateDensity(input.pos.xyz)), 1.0f);
  return input.color;
}
