
#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

float smoothingKernel(float dist) {
  float radius = pc.worldDataBufferAddress.smoothRadius;
  if (dist >= radius)
    return 0;
  float volume = (PI * pow(radius, 4)) / 6;
  return (radius - dist) * (radius - dist) / volume;
}

float calculateDensity(float3 p) {
  float density = 0;
  for (int i = 0; i < pc.particleCount; i++) {
    float dist = length(pc.particleBufferAddress[i].position - p);
    float influence = smoothingKernel(dist);
    density += mass * influence;
  }
  return density;
}

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;
  Particle* p = pc.particleBufferAddress + index;
  p.density = calculateDensity(p.position);
}
