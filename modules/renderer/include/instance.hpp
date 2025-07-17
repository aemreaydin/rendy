#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace rendy::renderer::vulkan {

class Instance {
  VkInstance _vk_instance;
  VkDebugUtilsMessengerCreateInfoEXT _vk_debug_utils_messenger_create_info;
  VkDebugUtilsMessengerEXT _vk_debug_utils_messenger;
  PFN_vkCreateDebugUtilsMessengerEXT _fn_create_debug_utils_messenger;
  PFN_vkDestroyDebugUtilsMessengerEXT _fn_destroy_debug_utils_messenger;

  void createDebugUtilsMessengerCreateInfo();
  auto initializeDebugUtilsMessenger() -> VkResult;
  void destroyDebugUtilsMessenger();

  [[nodiscard]] static auto validateExtensions(const std::vector<const char *> &required_extensions) -> bool;
  [[nodiscard]] static auto validateLayers(const std::vector<const char *> &required_layers) -> bool;

public:
  [[nodiscard]] auto Initialize(std::span<const char *const>) -> bool;
  void Destroy();

  [[nodiscard]] auto Get() const -> VkInstance;
};

} // namespace rendy::renderer::vulkan
