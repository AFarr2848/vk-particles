// This has to handle:
//  - Uploading vertices and indices
//  - Choosing textures
//  - Input
//  - Configuring drawing settings
//  - Updating transforms
//

#include "engine/World.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include "engine/Camera.hpp"
#include "engine/InputHelper.hpp"
#include "engine/Shapes.hpp"
#include "engine/Structs.hpp"
uint32_t fe_World::addShape(fe_Shape shape,
                            glm::mat4 transform,
                            fe_Material material) {
  shapes.push_back({{shape}, {material}});
  transforms.push_back(transform);
  return shapes.size() - 1;
}

void fe_World::transformShapes() {}

void fe_World::processInput(fe_FrameContext frameContext) {
  /*
  if (inputHelper.isKeyDown('A')) {
    camera.processKeyboard(LEFT, frameContext.deltaTime);
  }
  if (inputHelper.isKeyDown('W'))
    camera.processKeyboard(FORWARD, frameContext.deltaTime);
  if (inputHelper.isKeyDown('D'))
    camera.processKeyboard(RIGHT, frameContext.deltaTime);
  if (inputHelper.isKeyDown('S'))
    camera.processKeyboard(BACKWARD, frameContext.deltaTime);

  if (inputHelper.isKeyDown(KEY_SPACE))
    camera.processKeyboard(UP, frameContext.deltaTime);
  if (inputHelper.isKeyDown(KEY_LEFT_CTRL))
    camera.processKeyboard(DOWN, frameContext.deltaTime);

  camera.ProcessMouseMovement(inputHelper.getMouseOffsets().x,
                              inputHelper.getMouseOffsets().y);
  */
}

void fe_World::prepareDraw(std::vector<fe_Vertex>& vertices,
                           std::vector<uint32_t>& indices,
                           std::vector<fe_DrawInfo>& drawInfos) {
  for (auto& pair : shapes) {
    drawInfos.push_back(
        {.indexCount = static_cast<uint32_t>(pair.first.indices.size()),
         .indexOffset = static_cast<uint32_t>(indices.size()),
         .vertexOffset = static_cast<uint32_t>(vertices.size()),
         .transformIndex = static_cast<uint32_t>(drawInfos.size()),
         .material = pair.second});

    vertices.insert(vertices.end(), pair.first.vertices.begin(),
                    pair.first.vertices.end());
    indices.insert(indices.end(), pair.first.indices.begin(),
                   pair.first.indices.end());
  }
}

fe_WorldData fe_World::getWorldData(fe_FrameContext frameContext) {
  // TODO: Proper carmar

  float aspect = static_cast<float>(frameContext.screenWidth) /
                 static_cast<float>(frameContext.screenHeight);
  fe_WorldData worldData = {
      .view = glm::mat4(1.0f),
      .model = glm::mat4(1.0f),
      .proj = glm::orthoZO(-aspect, aspect, -1.0f, 1.0f, 1000.0f, 0.1f),

      //.proj =
      //   glm::perspectiveZO(glm::radians(45.0f),
      //                    static_cast<float>(frameContext.screenWidth) /
      //                      static_cast<float>(frameContext.screenHeight),
      //                1000.0f, 0.1f),
      .cameraPos = glm::vec3(0.0f),
      .aspect = aspect};
  worldData.proj[1][1] *= -1;

  return worldData;
}

void fe_World::createShapes() {}
