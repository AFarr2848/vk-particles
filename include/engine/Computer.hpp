#pragma once

class fe_VulkanContext;
class fe_TimingData;
class fe_ShaderManager;
class fe_BufferManager;
class fe_Computer {
 public:
  fe_Computer(fe_VulkanContext& ctx,
              fe_TimingData& tim,
              fe_BufferManager& bufferMan,
              fe_ShaderManager& shaderMan)
      : ctx(ctx), tim(tim), bufferMan(bufferMan), shaderMan(shaderMan) {};

  /**
   * @brief Dispatches the shader that fills the particle buffer
   */
  void initParticles();

  void startCompute();

 private:
  void placeParticleBarrier();
  void sortParticles();

  fe_VulkanContext& ctx;
  fe_TimingData& tim;
  fe_BufferManager& bufferMan;
  fe_ShaderManager& shaderMan;
};
