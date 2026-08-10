#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct fe_Vertex {
  alignas(16) glm::vec3 pos;
  alignas(16) glm::vec3 normal;
  alignas(16) glm::vec2 uv;
};

struct fe_Particle {
  alignas(16) glm::vec3 pos;
  alignas(16) glm::vec3 vel;
};

struct fe_PushConstants {
  uint64_t particleBufAddress;
  uint64_t worldBufAddress;
  float deltaTime;
  uint32_t particleCount;
};

struct fe_Material {
  std::string shader;
  std::string texture;
};

struct fe_DrawInfo {
  uint32_t indexCount;
  uint32_t indexOffset;
  uint32_t vertexOffset;
  uint32_t transformIndex;
  fe_Material material;
};

struct fe_Texture {
  std::string name;
  vk::raii::Image image = nullptr;
  vk::raii::DeviceMemory memory = nullptr;
  vk::raii::ImageView view = nullptr;
  vk::raii::Sampler sampler = nullptr;
};

struct fe_WorldData {
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 proj;
  glm::vec3 cameraPos;
  float aspect;
  float particleSize;
  float smoothRadius;
  float densityDrawConst;
  float targetDensity;
  float pressureMultiplier;
  float gravity;
  float collisionDampingConst;
  float drag;
};

struct fe_FrameContext {
  float deltaTime;
  float totalTime;
  uint64_t frameIndex;
  uint64_t screenWidth;
  uint64_t screenHeight;
};

struct fe_GUIValues {
  float smoothRadius;
  float particleSize;
  float densityDrawConst;
  float targetDensity;
  float pressureMultiplier;
  int drawMode;
  float gravity;
  float collisionDampingConst;
  float drag;
};
