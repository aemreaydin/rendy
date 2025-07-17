#include "instance.hpp"
#include <magic_enum/magic_enum.hpp>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace rendy::renderer::vulkan {

constexpr const char *kAppName = "Rendy";
constexpr const char *kEngineName = "Rendy Engine";

constexpr const char *kPortabilityExtension = "VK_KHR_portability_enumeration";
constexpr const char *kValidationLayer = "";

auto Instance::Initialize(std::span<const char *const> window_extensions)
    -> bool {

  VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                .pNext = nullptr,
                                .pApplicationName = kAppName,
                                .pEngineName = kEngineName,
                                .apiVersion = VK_MAKE_VERSION(1, 4, 0)};

  std::vector<const char *> extension_names;
  extension_names.assign(window_extensions.begin(), window_extensions.end());

  spdlog::info("{}", fmt::join(extension_names, ", "));
  VkInstanceCreateInfo instance_create_info{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef __APPLE__
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = static_cast<uint32_t>(extension_names.size()),
      .ppEnabledExtensionNames = extension_names.data()};
  auto result = vkCreateInstance(&instance_create_info, nullptr, &_vk_instance);
  if (result != VK_SUCCESS) {
    spdlog::error("Failed to create Vulkan instance({}).",
                  magic_enum::enum_name(result));
    return false;
  }

  spdlog::info("Vulkan instance created.");
  return true;
}

auto Instance::Get() const -> VkInstance { return _vk_instance; }

} // namespace rendy::renderer::vulkan
