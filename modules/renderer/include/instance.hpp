#include <span>
#include <vulkan/vulkan.h>

namespace rendy::renderer::vulkan {

class Instance {
  VkInstance _vk_instance;

public:
  [[nodiscard]] auto Initialize(std::span<const char *const>) -> bool;

  [[nodiscard]] auto Get() const -> VkInstance;
};

} // namespace rendy::renderer::vulkan
