#pragma once

#include "vulkan/vulkan.hpp"

namespace rendy::graphics::vulkan {

class PhysicalDevice {
  vk::PhysicalDevice _vk_physical_device;
  vk::PhysicalDeviceFeatures _vk_features;
  vk::PhysicalDeviceProperties _vk_properties;
  vk::PhysicalDeviceMemoryProperties _vk_memory_properties;
  std::vector<vk::QueueFamilyProperties> _vk_queue_family_properties;

  void queryDeviceInfo();

public:
  [[nodiscard]] auto Initialize(class Instance &instance) -> vk::Bool32;
  void Destroy();

  [[nodiscard]] auto Get() const -> vk::PhysicalDevice;
};

} // namespace rendy::graphics::vulkan
