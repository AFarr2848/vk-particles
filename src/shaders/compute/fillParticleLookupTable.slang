

#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;

  uint tableMask = (pc.particleCount << 1) - 1;

  Particle* thisParticle = pc.worldDataBufferAddress.particleBufferAddress + index;

  uint thisBucket = thisParticle.cellHash & tableMask;

  if (index == 0) {
    pc.worldDataBufferAddress.particleLookupBufferAddress[thisBucket] = index;
    return;
  }

  Particle* prevParticle = pc.worldDataBufferAddress.particleBufferAddress + index - 1;
  uint prevBucket = prevParticle.cellHash & tableMask;

  if (thisBucket != prevBucket) {
    pc.worldDataBufferAddress.particleLookupBufferAddress[thisBucket] = index;
  }
}
