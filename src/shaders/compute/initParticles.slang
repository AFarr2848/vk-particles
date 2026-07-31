#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

static const float PI = 3.14159265359;

uint hash(uint x) {
  x += (x << 10u);
  x ^= (x >> 6u);
  x += (x << 3u);
  x ^= (x >> 11u);
  x += (x << 15u);
  return x;
}

float random01(uint seed) {
  return float(hash(seed)) / 4294967295.0;
}

void spawnParticle(uint index) {
  uint frameSeed = uint(pc.deltaTime * 1000.0);

  float u1 = random01(index * 3u + 0u + frameSeed);
  float u2 = random01(index * 3u + 1u + frameSeed);
  float u3 = random01(index * 3u + 2u + frameSeed);

  Particle p = (Particle)0;

  float theta = u1 * 2.0 * PI;
  float phi = acos(2.0 * u2 - 1.0);
  float r = pow(u3, 1.0 / 3.0);
  float sinPhi = sin(phi);

  float3 dir = float3(sinPhi * cos(theta), sinPhi * sin(theta), cos(phi));

  float spawnRadius = 1.0;
  float speed = 0.5;

  p.position = dir * r * spawnRadius;
  p.velocity = dir * speed;

  pc.particleBufferAddress[index] = p;
}

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;
  spawnParticle(index);
}

