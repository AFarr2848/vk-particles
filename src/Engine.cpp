#include "engine/Engine.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vulkan/vulkan_hpp_macros.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "engine/Computer.hpp"
#include "engine/GUI.hpp"
#include "engine/Renderer.hpp"
#include "engine/Structs.hpp"
#include "engine/Swapchain.hpp"
#include "engine/Timing.hpp"
#include "engine/VulkanContext.hpp"
#include "engine/Window.hpp"
#include "engine/World.hpp"
#include "engine/managers/BufferManager.hpp"
#include "engine/managers/ShaderManager.hpp"
#include "engine/managers/TextureManager.hpp"
#include "vulkan/vulkan.hpp"

fe_Engine::~fe_Engine() = default;
fe_Engine::fe_Engine() = default;

void fe_Engine::startEngine() {
  inputHelper = std::make_unique<fe_InputHelper>();
  win = std::make_unique<fe_Window>(*inputHelper);
  ctx = std::make_unique<fe_VulkanContext>(*win);
  swp = std::make_unique<fe_Swapchain>(*win, *ctx);
  tim = std::make_unique<fe_TimingData>(*ctx, *swp);
  shaderMan = std::make_unique<fe_ShaderManager>(*ctx);
  bufferMan = std::make_unique<fe_BufferManager>(*ctx);
  texMan = std::make_unique<fe_TextureManager>(*ctx, *tim);
  world = std::make_unique<fe_World>(*inputHelper);
  cmp = std::make_unique<fe_Computer>(*ctx, *tim, *bufferMan, *shaderMan);
  gui = std::make_unique<fe_GUI>(*ctx, *win, *swp);
  renderer = std::make_unique<fe_Renderer>(*ctx, *swp, *tim, *cmp, *bufferMan,
                                           *shaderMan, *texMan, *gui);

  win->init();
  ctx->init();
  swp->init();
  tim->init();
  world->init();
  gui->init();

  texMan->loadTextures();
  // texMan->addTextureFromColor(glm::vec3(1.0f, 0.0f, 1.0f), "pink");
  // texMan->addTextureFromColor(glm::vec3(1.0f, 0.0f, 0.0f), "red");

  std::cout << "Loading shader modules..." << std::endl;
  shaderMan->loadShaderModule(
      "initParticles_comp", "build/shaders/initParticles_comp.spv",
      vk::ShaderStageFlagBits::eCompute, texMan->texSetLayout);
  shaderMan->loadShaderModule(
      "particles_comp", "build/shaders/particles_comp.spv",
      vk::ShaderStageFlagBits::eCompute, texMan->texSetLayout);

  shaderMan->loadShaderModule(
      "drawParticles_frag", "build/shaders/drawParticles_frag.spv",
      vk::ShaderStageFlagBits::eFragment, texMan->texSetLayout);
  shaderMan->loadShaderModule(
      "drawParticles_vert", "build/shaders/drawParticles_vert.spv",
      vk::ShaderStageFlagBits::eVertex, texMan->texSetLayout);

  shaderMan->loadShaderModule(
      "drawDensity_frag", "build/shaders/drawDensity_frag.spv",
      vk::ShaderStageFlagBits::eFragment, texMan->texSetLayout);
  shaderMan->loadShaderModule(
      "drawDensity_vert", "build/shaders/drawDensity_vert.spv",
      vk::ShaderStageFlagBits::eVertex, texMan->texSetLayout);

  std::vector<fe_Vertex> vertices = {};
  std::vector<uint32_t> indices = {};

  std::cout << "Loading meshes..." << std::endl;
  world->prepareDraw(vertices, indices, drawInfos);

  std::cout << "Creating buffers..." << std::endl;
  // bufferMan->createMeshBuffer(vertices, indices);
  // bufferMan->createTransformBuffer(sizeof(glm::mat4) *
  //                                 world->transforms.size());
  bufferMan->createWorldBuffer();
  bufferMan->createParticleBuffer();

  ctx->createPipelineLayout();

  cmp->initParticles();
}

void fe_Engine::run() {
  while (!glfwWindowShouldClose(win->window)) {
    glfwPollEvents();
    frameContext = {

        .deltaTime = tim->deltaTime,
        .totalTime = tim->currentTime,
        .frameIndex = tim->currentFrame,
        .screenWidth = swp->swapChainExtent.width,
        .screenHeight = swp->swapChainExtent.height

    };

    inputHelper->updateInputs();
    world->processInput(frameContext);
    world->guiValues = gui->getGUIData();
    // world->transformShapes();
    fe_WorldData worldData = world->getWorldData(frameContext);

    // bufferMan->updateTransformBuffer(world->transforms);
    bufferMan->updateWorldBuffer(worldData);
    renderer->drawFrame();
  }
  ctx->device.waitIdle();
  gui->cleanup();
}
