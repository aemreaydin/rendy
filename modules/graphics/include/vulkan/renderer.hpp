#pragma once

#include "instance.hpp"
#include "physical_device.hpp"

#include <memory>

namespace rendy::graphics::vulkan {

class Renderer {
  std::unique_ptr<Instance> _instance;
  std::unique_ptr<PhysicalDevice> _physical_device;

public:
  void Initialize();
  void Destroy();
};

} // namespace rendy::graphics::vulkan
