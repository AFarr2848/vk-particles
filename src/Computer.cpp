#include "engine/Computer.hpp"
#include "Config.hpp"
#include "engine/Structs.hpp"
#include "engine/Timing.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/managers/BufferManager.hpp"
#include "engine/managers/ShaderManager.hpp"
#include "vulkan/vulkan.hpp"

void fe_Computer::placeParticleBarrier() {
  vk::CommandBuffer cmd = tim.getCurrentCmdBuffer();

  vk::MemoryBarrier2 globalBarrier{
      .srcStageMask = vk::PipelineStageFlagBits2::eComputeShader |
                      vk::PipelineStageFlagBits2::eTransfer,
      .srcAccessMask = vk::AccessFlagBits2::eShaderWrite |
                       vk::AccessFlagBits2::eTransferWrite,

      .dstStageMask = vk::PipelineStageFlagBits2::eComputeShader,
      .dstAccessMask =
          vk::AccessFlagBits2::eShaderRead | vk::AccessFlagBits2::eShaderWrite};

  vk::DependencyInfo dependencyInfo{.memoryBarrierCount = 1,
                                    .pMemoryBarriers = &globalBarrier};

  cmd.pipelineBarrier2(dependencyInfo);
}

void fe_Computer::sortParticles() {
  vk::CommandBuffer cmd = tim.getCurrentCmdBuffer();
  cmd.bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                     shaderMan.getShader("bitonicSort_comp"));
  for (uint32_t k = 2; k <= MAX_PARTICLES; k <<= 1) {
    for (uint32_t j = k >> 1; j > 0; j >>= 1) {
      fe_PushConstants pc = {.worldBufAddress = bufferMan.worldBufferAddress,
                             .deltaTime = tim.deltaTime,
                             .particleCount = MAX_PARTICLES,
                             .data1 = j,
                             .data2 = k};
      cmd.pushConstants(ctx.pipelineLayout,
                        vk::ShaderStageFlagBits::eCompute |
                            vk::ShaderStageFlagBits::eFragment |
                            vk::ShaderStageFlagBits::eVertex,
                        0, sizeof(pc), &pc);

      cmd.dispatch((MAX_PARTICLES / 2 + 255) / 256, 1, 1);

      placeParticleBarrier();
    }
  }
}

void fe_Computer::startCompute() {
  fe_PushConstants pc{.worldBufAddress = bufferMan.worldBufferAddress,
                      .deltaTime = tim.deltaTime,
                      .particleCount = MAX_PARTICLES};

  vk::CommandBuffer cmd = tim.getCurrentCmdBuffer();
  uint32_t localSizeX = 256;
  uint32_t groupCountX = (MAX_PARTICLES + localSizeX - 1) / localSizeX;

  cmd.pushConstants(ctx.pipelineLayout,
                    vk::ShaderStageFlagBits::eCompute |
                        vk::ShaderStageFlagBits::eFragment |
                        vk::ShaderStageFlagBits::eVertex,
                    0, sizeof(pc), &pc);

  // calculate densities
  cmd.bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                     shaderMan.getShader("calculateDensities_comp"));
  cmd.dispatch(groupCountX, 1, 1);
  placeParticleBarrier();

  // particle pos hashes
  cmd.bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                     shaderMan.getShader("setParticlePositionHashes_comp"));
  cmd.dispatch(groupCountX, 1, 1);
  placeParticleBarrier();

  // sort particles
  sortParticles();
  placeParticleBarrier();

  cmd.pushConstants(ctx.pipelineLayout,
                    vk::ShaderStageFlagBits::eCompute |
                        vk::ShaderStageFlagBits::eFragment |
                        vk::ShaderStageFlagBits::eVertex,
                    0, sizeof(pc), &pc);

  // fill lookup table
  cmd.bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                     shaderMan.getShader("fillParticleLookupTable_comp"));
  cmd.dispatch(groupCountX, 1, 1);
  placeParticleBarrier();

  // move particles
  cmd.bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                     shaderMan.getShader("particles_comp"));
  cmd.dispatch(groupCountX, 1, 1);

  vk::BufferMemoryBarrier2 particleBarrier{
      .srcStageMask = vk::PipelineStageFlagBits2::eComputeShader,
      .srcAccessMask = vk::AccessFlagBits2::eShaderWrite,

      .dstStageMask = vk::PipelineStageFlagBits2::eVertexShader,
      .dstAccessMask = vk::AccessFlagBits2::eShaderRead,

      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

      .buffer = bufferMan.particleBuffer,
      .offset = 0,
      .size = vk::WholeSize

  };

  vk::DependencyInfo dependencyInfo{.bufferMemoryBarrierCount = 1,
                                    .pBufferMemoryBarriers = &particleBarrier};

  cmd.pipelineBarrier2(dependencyInfo);
}

void fe_Computer::initParticles() {
  std::unique_ptr<vk::raii::CommandBuffer> cmd = tim.beginSingleTimeCommands();

  fe_PushConstants pc{.worldBufAddress = bufferMan.worldBufferAddress,
                      .deltaTime = tim.deltaTime,
                      .particleCount = MAX_PARTICLES};

  uint32_t localSizeX = 256;
  uint32_t groupCountX = (MAX_PARTICLES + localSizeX - 1) / localSizeX;

  cmd->pushConstants(ctx.pipelineLayout,
                     vk::ShaderStageFlagBits::eCompute |
                         vk::ShaderStageFlagBits::eFragment |
                         vk::ShaderStageFlagBits::eVertex,
                     0, sizeof(pc), &pc);
  cmd->bindShadersEXT(vk::ShaderStageFlagBits::eCompute,
                      shaderMan.getShader("initParticles_comp"));

  cmd->dispatch(groupCountX, 1, 1);

  tim.endSingleTimeCommands(*cmd);
}
