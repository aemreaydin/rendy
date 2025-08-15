#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace rendy::gfx::vulkan {

class Instance {
  uint32_t _vk_api_version{0};
  vk::Instance _vk_instance{nullptr};
  vk::DebugUtilsMessengerCreateInfoEXT _vk_debug_utils_messenger_create_info;
  vk::DebugUtilsMessengerEXT _vk_debug_utils_messenger;

  void createDebugUtilsMessengerCreateInfo();
  [[nodiscard]] auto initializeDebugUtilsMessenger() -> vk::Bool32;

  [[nodiscard]] static auto validateExtensions(const std::vector<const char *> &required_extensions) -> bool;
  [[nodiscard]] static auto validateLayers(const std::vector<const char *> &required_layers) -> bool;

public:
  [[nodiscard]] auto Initialize(std::span<const char *const> window_extensions) -> vk::Bool32;
  void Destroy() const;

  [[nodiscard]] auto Get() const -> vk::Instance;
};

} // namespace rendy::gfx::vulkan
