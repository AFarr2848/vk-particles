#include "include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

[shader("vertex")]
VSOutput vertexMain(uint vertexID: SV_VertexID) {
  VSOutput output;

  output.uv = float2((vertexID << 1) & 2, vertexID & 2);
  output.pos = float4(output.uv.x * 2.0 - 1.0, output.uv.y * 2.0 - 1.0, 0.0, 1.0);

  return output;
}

float smoothingKernel(float dist) {
  const float smoothRadius = pc.worldDataBufferAddress.smoothRadius;
  float volume = PI * pow(smoothRadius, 8) / 4;
  float value = max(0, smoothRadius * smoothRadius - dist * dist);
  return value * value * value / volume;
}

float calculateDensity(float3 p) {
  float density = 0;
  float mass = 1;
  for (int i = 0; i < pc.particleCount; i++) {
    float dist = length(pc.particleBufferAddress[i].position - p);
    float influence = smoothingKernel(dist);
    density += mass * influence;
  }
  return density;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target {
  float aspect = pc.worldDataBufferAddress.aspect;
  float3 worldPos = float3((input.uv.x * 2.0 - 1.0) * aspect, -(input.uv.y * 2.0 - 1.0), 0.0);
  float density = calculateDensity(worldPos);

  float visualDensity =
      density *
      pc.worldDataBufferAddress
          .densityDrawConst;  // Tweak this value until it looks smooth!
                              // 
                              // 
  return float4(float3(visualDensity), 1.0f);
}
