#pragma once

#include "engine/Structs.hpp"
class fe_Window;
class fe_VulkanContext;
class fe_TimingData;
class fe_Swapchain;
class fe_BufferManager;
class fe_ShaderManager;
class fe_World;
class fe_InputHelper;
class fe_TextureManager;
class fe_Renderer;
class fe_Computer;
class fe_GUI;

class fe_Engine {
 public:
  fe_Engine();
  ~fe_Engine();

  // vroom
  void startEngine();
  void run();

 private:
  std::vector<fe_DrawInfo> drawInfos = {};

  fe_FrameContext frameContext;
  fe_GUIValues guiValues;

  std::unique_ptr<fe_Window> win;
  std::unique_ptr<fe_VulkanContext> ctx;
  std::unique_ptr<fe_Swapchain> swp;
  std::unique_ptr<fe_TimingData> tim;
  std::unique_ptr<fe_ShaderManager> shaderMan;
  std::unique_ptr<fe_BufferManager> bufferMan;
  std::unique_ptr<fe_TextureManager> texMan;
  std::unique_ptr<fe_World> world;
  std::unique_ptr<fe_InputHelper> inputHelper;
  std::unique_ptr<fe_Renderer> renderer;
  std::unique_ptr<fe_Computer> cmp;
  std::unique_ptr<fe_GUI> gui;
};
