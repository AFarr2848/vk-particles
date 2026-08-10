#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

uint hash(uint x) {
  x += (x << 10u);
  x ^= (x >> 6u);
  x += (x << 3u);
  x ^= (x >> 11u);
  x += (x << 15u);
  return x;
}

float nextRandom(inout uint seed) {
  seed = hash(seed);
  return float(seed) / 4294967295.0;
}

void spawnParticle(uint index) {
  uint frameSeed = uint(pc.deltaTime * 1000.0);

  uint rngState = hash(index ^ frameSeed);

  float u1 = nextRandom(rngState);
  float u2 = nextRandom(rngState);
  float u3 = nextRandom(rngState);
  float u4 = nextRandom(rngState);
  float u5 = nextRandom(rngState);
  float u6 = nextRandom(rngState);

  Particle p = (Particle)0;

  float theta = u1 * 2.0 * PI;
  float phi = acos(2.0 * u2 - 1.0);
  float r = pow(u3, 1.0 / 3.0);
  float sinPhi = sin(phi);

  float3 dir = float3(sinPhi * cos(theta), sinPhi * sin(theta), 0);

  float velTheta = u4 * 2.0 * PI;
  float velPhi = acos(2.0 * u5 - 1.0);
  float3 velDir = float3(sin(velPhi) * cos(velTheta), sin(velPhi) * sin(velTheta), 0);

  float spawnRadius = 1.0;
  float speed = 0.5;

  p.position = dir * r * spawnRadius;
  p.velocity = float3(0);

  pc.particleBufferAddress[index] = p;
}

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;
  spawnParticle(index);
}

