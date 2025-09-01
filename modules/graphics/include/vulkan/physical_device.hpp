#pragma once

#include "queue.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;

  [[nodiscard]] auto IsAdequate() const -> bool { return !formats.empty() && !present_modes.empty(); }
};

class PhysicalDevice {
  vk::PhysicalDevice _vk_physical_device;
  vk::PhysicalDeviceFeatures _vk_features;
  vk::PhysicalDeviceProperties _vk_properties;
  vk::PhysicalDeviceMemoryProperties _vk_memory_properties;
  std::vector<vk::QueueFamilyProperties> _vk_queue_family_properties;
  QueueFamilyIndices _queue_family_indices;
  SwapChainSupportDetails _swapchain_support;

  void queryDeviceInfo();
  [[nodiscard]] static auto findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> QueueFamilyIndices;
  [[nodiscard]] static auto querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
      -> SwapChainSupportDetails;
  [[nodiscard]] static auto checkDeviceExtensionSupport(vk::PhysicalDevice device) -> bool;
  [[nodiscard]] static auto isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> bool;
  [[nodiscard]] static auto scoreDevice(vk::PhysicalDevice device) -> uint32_t;

public:
  [[nodiscard]] auto Initialize(class Instance &instance, vk::SurfaceKHR surface) -> bool;
  void Destroy();

  [[nodiscard]] auto Get() const -> vk::PhysicalDevice;
  [[nodiscard]] auto GetQueueFamilyIndices() const -> const QueueFamilyIndices &;
  [[nodiscard]] auto GetSwapChainSupport() const -> const SwapChainSupportDetails &;
  [[nodiscard]] auto GetFeatures() const -> const vk::PhysicalDeviceFeatures &;
  [[nodiscard]] auto GetProperties() const -> const vk::PhysicalDeviceProperties &;
  [[nodiscard]] auto GetMemoryProperties() const -> const vk::PhysicalDeviceMemoryProperties &;
};

} // namespace rendy::graphics::vulkan
