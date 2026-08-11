#include "engine/GUI.hpp"
#include "Config.hpp"
#include "engine/Structs.hpp"
#include "engine/Swapchain.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/Window.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.hpp"

void fe_GUI::startImgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = nullptr;

  ImGui::StyleColorsDark();
  // createImguiDscPool();

  VkFormat colorFormat =
      static_cast<VkFormat>(swp.swapChainSurfaceFormat.format);
  ImGui_ImplVulkan_InitInfo initInfo = {
      .ApiVersion = VK_API_VERSION_1_3,
      .Instance = *ctx.instance,
      .PhysicalDevice = *ctx.physicalDevice,
      .Device = *ctx.device,
      .QueueFamily = ctx.queueIndex,
      .Queue = *ctx.graphicsQueue,
      .DescriptorPool = *dscPool,
      .MinImageCount = (uint32_t)swp.swapChainImages.size(),
      .ImageCount = (uint32_t)swp.swapChainImages.size(),
      .PipelineInfoMain =
          {.PipelineRenderingCreateInfo =
               {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats = &colorFormat,
                .depthAttachmentFormat = VK_FORMAT_D32_SFLOAT}},
      .UseDynamicRendering = true

  };
  ImGui_ImplVulkan_Init(&initInfo);
  ImGui_ImplGlfw_InitForVulkan(win.window, true);

  guiValues.particleSize = 0.005;
  guiValues.densityDrawConst = 0.15;
  guiValues.pressureMultiplier = 0.05;
  guiValues.gravity = 0.00;
  guiValues.collisionDampingConst = 0.95;
  guiValues.particleSpacing = 0.001;
  guiValues.drag = 0.999;
}

void fe_GUI::renderImgui(vk::raii::CommandBuffer& cmd) {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  drawRightSidePanel();

  ImGui::Render();

  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd

  );
}

fe_GUIValues fe_GUI::getGUIData() {
  return guiValues;
}

void fe_GUI::resizeGUI() {
  /*
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)swp.swapChainExtent.width,
                          (float)swp.swapChainExtent.height);
  */
}

void fe_GUI::drawRightSidePanel() {
  ImGuiIO& io = ImGui::GetIO();
  const float panelWidth = 300;

  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImVec2 viewportPos = viewport->Pos;
  ImVec2 viewportSize = viewport->Size;

  ImGui::SetNextWindowPos(
      ImVec2(viewportPos.x + viewportSize.x - panelWidth, viewportPos.y),
      ImGuiCond_Once);

  ImGui::SetNextWindowSize(ImVec2(panelWidth, viewportSize.y), ImGuiCond_Once);

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoCollapse;

  ImGui::Begin("Inspector", nullptr, flags);
  ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
              io.Framerate);
  ImGui::Text("%d particles", MAX_PARTICLES);

  ImGui::SeparatorText("Draw Modes");
  ImGui::RadioButton("Particles", &guiValues.drawMode, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Density", &guiValues.drawMode, 1);
  ImGui::SameLine();
  ImGui::RadioButton("Both", &guiValues.drawMode, 3);

  ImGui::SeparatorText("Physics Values");
  ImGui::DragFloat("particleSpacing", &guiValues.particleSpacing, 0.0001,
                   0.000001);
  ImGui::DragFloat("pressureMultiplier", &guiValues.pressureMultiplier, 0.001);
  ImGui::DragFloat("gravity", &guiValues.gravity, 0.001);
  ImGui::DragFloat("collisionDampingConst", &guiValues.collisionDampingConst,
                   0.01, 0, 1);
  ImGui::DragFloat("drag", &guiValues.drag, 0.001, 0, 1);

  ImGui::SeparatorText("Draw Values");
  ImGui::DragFloat("densityDrawConst", &guiValues.densityDrawConst, 0.0001);
  ImGui::DragFloat("particleSize", &guiValues.particleSize, 0.001, 0, 0.5);

  ImGui::End();
}

void fe_GUI::createDscPool() {
  vk::DescriptorPoolSize poolSizes[] = {
      {.type = vk::DescriptorType::eSampledImage,
       .descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
      {.type = vk::DescriptorType::eSampler,
       .descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE}};

  vk::DescriptorPoolCreateInfo createInfo = {
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = poolSizes[0].descriptorCount + poolSizes[1].descriptorCount,
      .poolSizeCount = 2,
      .pPoolSizes = poolSizes};

  dscPool = ctx.device.createDescriptorPool(createInfo);
}

void fe_GUI::cleanup() {
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
