#pragma once
class fe_BufferManager;
class fe_ShaderManager;
class fe_Swapchain;
class fe_VulkanContext;
class fe_TimingData;
class fe_TextureManager;
class fe_Computer;

class fe_Renderer {
 public:
  fe_Renderer(fe_VulkanContext& ctx,
              fe_Swapchain& swp,
              fe_TimingData& tim,
              fe_Computer& cmp,
              fe_BufferManager& bufferMan,
              fe_ShaderManager& shaderMan,
              fe_TextureManager& texMan)
      : ctx(ctx),
        swp(swp),
        tim(tim),
        cmp(cmp),
        bufferMan(bufferMan),
        shaderMan(shaderMan),
        texMan(texMan) {};
  /**
   * @brief Handles timing, increments frame stuff, inits and submits the
   * command buffer, and calls buffer updates and command records
   */
  void drawFrame();

  void recordCommandBuffer(uint32_t imageIndex);

  /**
   * @brief Does all the BS to the command buffer that's required in absence of
   * a pipeline
   */
  void configCommandBuffer();

 private:
  fe_BufferManager& bufferMan;
  fe_ShaderManager& shaderMan;
  fe_TextureManager& texMan;
  fe_Computer& cmp;
  fe_Swapchain& swp;
  fe_VulkanContext& ctx;
  fe_TimingData& tim;
};
