#include "engine/Computer.hpp"
#include "Config.hpp"
#include "engine/Structs.hpp"
#include "engine/Timing.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/managers/BufferManager.hpp"
#include "engine/managers/ShaderManager.hpp"

void fe_Computer::startCompute() {
  fe_PushConstants pc{.particleBufAddress = bufferMan.particleBufferAddress,
                      .worldBufAddress = bufferMan.worldBufferAddress,
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

  fe_PushConstants pc{.particleBufAddress = bufferMan.particleBufferAddress,
                      .worldBufAddress = bufferMan.worldBufferAddress,
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
