#include "engine/managers/ShaderManager.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_raii.hpp>
#include "engine/Structs.hpp"
#include "engine/Timing.hpp"
#include "engine/VulkanContext.hpp"
#include "vulkan/vulkan.hpp"

void fe_ShaderManager::loadShaderModule(
    const std::string& name,
    const std::string& filePath,
    vk::ShaderStageFlagBits stage,
    vk::raii::DescriptorSetLayout& texSetLayout) {
  std::cout << "Loading shader module " << name << " ..." << std::endl;
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open shader file: " + filePath);
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
  file.seekg(0);
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
  file.close();

  // prolly bad to have 2 of these
  vk::PushConstantRange pcRange = {
      .stageFlags = vk::ShaderStageFlagBits::eVertex |
                    vk::ShaderStageFlagBits::eFragment |
                    vk::ShaderStageFlagBits::eCompute,
      .offset = 0,
      .size = sizeof(fe_PushConstants)

  };

  vk::DescriptorSetLayout setLayout = texSetLayout;
  vk::ShaderStageFlagBits nextStage;
  std::string entryName;
  switch (stage) {
    case vk::ShaderStageFlagBits::eVertex:
      entryName = "vertexMain";
      nextStage = vk::ShaderStageFlagBits::eFragment;
      break;
    case vk::ShaderStageFlagBits::eFragment:
      entryName = "fragmentMain";
      nextStage = vk::ShaderStageFlagBits{};
      break;
    case vk::ShaderStageFlagBits::eCompute:
      entryName = "computeMain";
      nextStage = vk::ShaderStageFlagBits{};
      break;

    default:
      throw std::runtime_error(
          "ERROR - Unhandled shader stage when loading shader modules!");
  }

  shaderRegistry.emplace(
      name,
      ctx.device.createShaderEXT({.stage = stage,
                                  .nextStage = nextStage,
                                  .codeType = vk::ShaderCodeTypeEXT::eSpirv,
                                  .codeSize = buffer.size() * sizeof(uint32_t),
                                  .pCode = buffer.data(),
                                  .pName = entryName.c_str(),
                                  .setLayoutCount = 1,
                                  .pSetLayouts = &setLayout,
                                  .pushConstantRangeCount = 1,
                                  .pPushConstantRanges = &pcRange}));
}

vk::ShaderEXT fe_ShaderManager::getShader(const std::string& name) {
  return shaderRegistry.at(name);
}
