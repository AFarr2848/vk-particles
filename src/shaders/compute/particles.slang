
// Lots of code from here is taken from Sebastian Lague's "Coding Adventure: Simulating Fluids"
// video
//
// Although i think thats fair cause this whole project was inspired by that lol

#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

void moveParticle(Particle* p) {
  float aspect = pc.worldDataBufferAddress.aspect;

  p.position.xyz += p.velocity.xyz * pc.deltaTime;
  // p.velocity.y -= 0.001f;
  // p.velocity *= 0.9999;
  

  if (p.position.x >= aspect) {
    p.position.x = aspect;
    p.velocity.x *= -1;
  }
  if (p.position.x <= -aspect) {
    p.position.x = -aspect;
    p.velocity.x *= -1;
  }
  if (p.position.y >= 1.0) {
    p.position.y = 1.0;
    p.velocity.y *= -1;
  }
  if (p.position.y <= -1.0) {
    p.position.y = -1.0;
    p.velocity.y *= -1;
  }
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
    float dist = length(pc.particleBufferAddress.position - p);
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

  moveParticle(p);
}
