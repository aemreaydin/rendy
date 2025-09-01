#pragma once

#include "device.hpp"
#include "instance.hpp"
#include "physical_device.hpp"
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

class Renderer {
  std::unique_ptr<vk::SurfaceKHR> _surface;
  std::unique_ptr<Instance> _instance;
  std::shared_ptr<PhysicalDevice> _physical_device;
  std::unique_ptr<VulkanDevice> _device;

public:
  void Initialize(SDL_Window &window);
  void Destroy();
};

} // namespace rendy::graphics::vulkan
