#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;

  if (index >= pc.particleCount)
    return;

  Particle p = pc.particleBufferAddress[index];

  p.position.xyz += p.velocity.xyz * pc.deltaTime;

  if (p.position.x >= 1.0) {
    p.position.x = 1.0;
    p.velocity.x *= -1;
  }
  if (p.position.x <= -1.0) {
    p.position.x = -1.0;
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

  pc.particleBufferAddress[index] = p;
}
