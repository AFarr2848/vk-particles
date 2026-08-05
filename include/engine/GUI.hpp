#pragma once
#include "engine/Structs.hpp"
class fe_VulkanContext;
class fe_Window;
class fe_Swapchain;

class fe_GUI {
 public:
  void init() {
    createDscPool();
    startImgui();
  }
  fe_GUI(fe_VulkanContext& ctx, fe_Window& win, fe_Swapchain& swp)
      : ctx(ctx), win(win), swp(swp) {};

  void renderImgui(vk::raii::CommandBuffer& cmd);
  void makePassesToAdd();
  void cleanup();
  fe_GUIValues getGUIData();
  void resizeGUI();

 private:
  fe_VulkanContext& ctx;
  fe_Swapchain& swp;
  fe_Window& win;

  vk::raii::DescriptorPool dscPool = nullptr;
  fe_GUIValues guiValues;

  void createDscPool();
  void startImgui();
  void drawRightSidePanel();
};
