#pragma once

#include "device.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

class RENDY_API Renderer {
  std::unique_ptr<vk::SurfaceKHR> _surface;
  std::unique_ptr<Instance> _instance;
  std::shared_ptr<PhysicalDevice> _physical_device;
  std::unique_ptr<VulkanDevice> _device;

public:
  void Initialize(GLFWwindow &window);
  void Destroy();
};

} // namespace rendy::graphics::vulkan
