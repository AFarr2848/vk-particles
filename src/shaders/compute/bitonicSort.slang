#include "../include/Structs.slang"

// Vulkan push constant block
[[vk::push_constant]]
PushConstants pc;

[shader("compute")]
[numthreads(256, 1, 1)]
void computeMain(uint3 threadId: SV_DispatchThreadID) {
  uint i = threadId.x;
  uint ixj = i ^ pc.data1;

  if (ixj > i) {
    // Determine sort direction (ascending or descending) for block k
    bool ascending = ((i & pc.data2) == 0);

    Particle* p1 = pc.worldDataBufferAddress.particleBufferAddress + i;
    Particle* p2 = pc.worldDataBufferAddress.particleBufferAddress + ixj;
    uint valA = p1.cellHash;
    uint valB = p2.cellHash;

    if ((ascending && valA > valB) || (!ascending && valA < valB)) {
      Particle tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
    }
  }
}
