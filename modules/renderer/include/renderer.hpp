#pragma once

#include "instance.hpp"
#include <memory>

namespace rendy::renderer::vulkan {

class Renderer {
  std::unique_ptr<Instance> _instance;

public:
  void Initialize();
  void Destroy();
};

} // namespace rendy::renderer::vulkan
