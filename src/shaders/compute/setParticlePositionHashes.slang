
#include "../include/Structs.slang"

[[vk::push_constant]]
PushConstants pc;

int getCellKey(int particleIndex) {
  Particle* p = pc.worldDataBufferAddress.particleBufferAddress + particleIndex;
  float cellSize = pc.worldDataBufferAddress.smoothRadius;
  int3 cellCoords = int3(floor(p.position / cellSize));
  // make sure to FIXME for 3d
  int cellHash = (cellCoords.x * 15823) + (cellCoords.y * 9737333);

  // Lookup table is twice as big as MAX_PARTICLES
  int lookupTableSize = pc.particleCount << 1;

  return cellHash & (lookupTableSize - 1);
}

[numthreads(256, 1, 1)]
void computeMain(uint3 dispatchThreadID: SV_DispatchThreadID) {
  uint index = dispatchThreadID.x;
  if (index >= pc.particleCount)
    return;

  Particle* p = pc.worldDataBufferAddress.particleBufferAddress + index;

  int lookupTableSize = pc.particleCount << 1;
  pc.worldDataBufferAddress.particleLookupBufferAddress[index] = 0xFFFFFFFF;
  pc.worldDataBufferAddress.particleLookupBufferAddress[lookupTableSize - 1 - index] = 0xFFFFFFFF;
  p.cellHash = getCellKey(index);
}
