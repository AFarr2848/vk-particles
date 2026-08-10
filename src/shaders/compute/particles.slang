
// Lots of code from here is taken from Sebastian Lague's "Coding Adventure: Simulating Fluids"
// video
//
// Although i think thats fair cause this whole project was inspired by that lol

#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

float smoothingKernelDerivative(float dist) {
  float radius = pc.worldDataBufferAddress.smoothRadius;
  if (dist >= radius)
    return 0;
  float scale = 12 / (pow(radius, 4) * PI);
  return (dist - radius) * scale;
}

float densityToPressure(float density) {
  float densityError = density - pc.worldDataBufferAddress.targetDensity;
  float pressure = densityError * pc.worldDataBufferAddress.pressureMultiplier;
  return pressure;
}

float calculateSharedPressure(float d1, float d2) {
  float p1 = densityToPressure(d1);
  float p2 = densityToPressure(d2);
  return (p1 + p2) / 2;
}

float3 calculatePressureForce(int particleIndex) {
  float3 pressureForce = float3(0);

  for (int i = 0; i < pc.particleCount; i++) {
    Particle* thisParticle = pc.particleBufferAddress + particleIndex;
    Particle* otherParticle = pc.particleBufferAddress + i;
    if (i == particleIndex)
      continue;
    float3 offset = thisParticle.position - otherParticle.position;
    float dist = length(offset);
    float3 dir = dist == 0 ? float3(1.0, 1.0, 0.0) : offset / dist;
    float slope = smoothingKernelDerivative(dist);
    float sharedPressure = calculateSharedPressure(thisParticle.density, otherParticle.density);
    pressureForce += sharedPressure * dir * slope * mass / thisParticle.density;
  }
  return pressureForce;
}

void moveParticle(int particleIndex) {
  float aspect = pc.worldDataBufferAddress.aspect;
  Particle* p = pc.particleBufferAddress + particleIndex;
  p.velocity.y -= pc.worldDataBufferAddress.gravity * pc.deltaTime;

  p.position.xyz += p.velocity.xyz * pc.deltaTime;

  p.velocity *= exp(-pc.worldDataBufferAddress.drag * pc.deltaTime);

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

void applyPressureForce(int particleIndex) {
  float aspect = pc.worldDataBufferAddress.aspect;
  Particle* p = pc.particleBufferAddress + particleIndex;

  float3 pressureForce = calculatePressureForce(particleIndex);
  float3 pressureAccel = pressureForce / p.density;
  p.velocity -= pressureAccel * pc.deltaTime * pc.worldDataBufferAddress.collisionDampingConst;
}

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;
  Particle* p = pc.particleBufferAddress + index;

  applyPressureForce(index);
  moveParticle(index);
}
