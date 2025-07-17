#include "instance.hpp"
#include <memory>

namespace rendy::renderer::vulkan {

class Renderer {
  std::unique_ptr<Instance> _instance;

public:
  void Initialize();
};

} // namespace rendy::renderer::vulkan
