#include "vulkan/physical_device.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/utils.hpp"
#include <set>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace rendy::graphics::vulkan {

auto PhysicalDevice::Initialize(Instance &instance, vk::SurfaceKHR surface) -> bool {
  auto physical_devices =
      VkCheckAndUnwrap(instance.Get().enumeratePhysicalDevices(), "Failed to enumerate physical devices");

  if (physical_devices.empty()) {
    spdlog::error("Couldn't find a physical device.");
    return false;
  }

  // Find the best suitable device, with fallback to any device if none are ideal
  vk::PhysicalDevice best_device;
  uint32_t best_score = 0;
  bool found_suitable = false;

  // First pass: try to find an ideal device
  for (const auto &device : physical_devices) {
    if (isDeviceSuitable(device, surface)) {
      uint32_t score = scoreDevice(device);
      if (score > best_score) {
        best_score = score;
        best_device = device;
        found_suitable = true;
      }
    }
  }

  // Fallback: if no suitable device found, pick the best scoring device anyway
  if (!found_suitable) {
    spdlog::warn("No ideal GPU found, selecting best available device with limited capabilities");
    best_score = 0;

    for (const auto &device : physical_devices) {
      uint32_t score = scoreDevice(device);
      spdlog::warn("Score {}", score);
      if (score > best_score) {
        best_score = score;
        best_device = device;
      }
    }
  }

  _vk_physical_device = best_device;
  _queue_family_indices = findQueueFamilies(_vk_physical_device, surface);
  _swapchain_support = querySwapChainSupport(_vk_physical_device, surface);
  queryDeviceInfo();

  // Log device capabilities and any missing features
  spdlog::info("Selected GPU: {}", _vk_properties.deviceName.data());

  spdlog::info("Graphics queue family: {}", _queue_family_indices.graphics_family);
  if (_queue_family_indices.compute_family.has_value()) {
    spdlog::info("Compute queue family: {}", _queue_family_indices.compute_family.value());
  } else {
    spdlog::info("No compute queue family found (compute shaders unavailable)");
  }

  if (found_suitable) {
    spdlog::info("Device has all required capabilities");
  } else {
    // Log what capabilities are missing for awareness
    if (!checkDeviceExtensionSupport(_vk_physical_device)) {
      spdlog::warn("Device doesn't support required extensions (e.g., VK_KHR_swapchain)");
    }

    if (!_swapchain_support.IsAdequate()) {
      spdlog::warn("Device has inadequate swapchain support");
    }

    if (_vk_features.samplerAnisotropy == vk::False) {
      spdlog::warn("Device doesn't support anisotropic filtering");
    }
  }

  return true;
}

void PhysicalDevice::Destroy() {}

auto PhysicalDevice::Get() const -> vk::PhysicalDevice { return _vk_physical_device; }

auto PhysicalDevice::GetQueueFamilyIndices() const -> const QueueFamilyIndices & { return _queue_family_indices; }

auto PhysicalDevice::GetSwapChainSupport() const -> const SwapChainSupportDetails & { return _swapchain_support; }

auto PhysicalDevice::GetFeatures() const -> const vk::PhysicalDeviceFeatures & { return _vk_features; }

auto PhysicalDevice::GetProperties() const -> const vk::PhysicalDeviceProperties & { return _vk_properties; }

auto PhysicalDevice::GetMemoryProperties() const -> const vk::PhysicalDeviceMemoryProperties & {
  return _vk_memory_properties;
}

void PhysicalDevice::queryDeviceInfo() {
  _vk_features = _vk_physical_device.getFeatures();
  _vk_properties = _vk_physical_device.getProperties();
  _vk_memory_properties = _vk_physical_device.getMemoryProperties();
  _vk_queue_family_properties = _vk_physical_device.getQueueFamilyProperties();
}

auto PhysicalDevice::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> QueueFamilyIndices {
  QueueFamilyIndices indices;

  auto queue_families = device.getQueueFamilyProperties();

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    const auto &queue_family = queue_families[i];

    auto present_support = VkCheckAndUnwrap(device.getSurfaceSupportKHR(i, surface), "Failed to get surface support");
    if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics && present_support != vk::False) {
      indices.graphics_family = i;
    }

    if (queue_family.queueFlags & vk::QueueFlagBits::eCompute) {
      indices.compute_family = i;
    }
  }

  return indices;
}

auto PhysicalDevice::querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
    -> SwapChainSupportDetails {
  SwapChainSupportDetails details;

  spdlog::info("Querying Device Capabilities");
  spdlog::debug("Physical device handle: {}", reinterpret_cast<void*>(static_cast<VkPhysicalDevice>(device)));
  spdlog::debug("Surface handle: {}", reinterpret_cast<void*>(static_cast<VkSurfaceKHR>(surface)));

  // First check if device supports presentation on any queue family for this surface
  bool presentation_supported = false;
  auto queue_families = device.getQueueFamilyProperties();
  for (uint32_t i = 0; i < queue_families.size(); i++) {
    if (VkCheckAndUnwrap(device.getSurfaceSupportKHR(i, surface), "Failed to get surface support") == vk::True) {
      presentation_supported = true;
      spdlog::debug("Queue family {} supports presentation", i);
    }
  }

  if (!presentation_supported) {
    spdlog::warn("Physical device doesn't support presentation for this surface");
    return details; // Return empty details
  }

  details.capabilities =
      VkCheckAndUnwrap(device.getSurfaceCapabilitiesKHR(surface), "Failed to get surface capabilities");

  spdlog::info("Querying Device Formats");
  details.formats = VkCheckAndUnwrap(device.getSurfaceFormatsKHR(surface), "Failed to get surface formats");

  details.present_modes =
      VkCheckAndUnwrap(device.getSurfacePresentModesKHR(surface), "Failed to get surface present modes");

  return details;
}

auto PhysicalDevice::checkDeviceExtensionSupport(vk::PhysicalDevice device) -> bool {
  static const std::vector<const char *> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  auto available_extensions =
      VkCheckAndUnwrap(device.enumerateDeviceExtensionProperties(), "Failed to enumerate device extensions");

  std::set<std::string> required_extensions(kDeviceExtensions.begin(), kDeviceExtensions.end());

  for (const auto &extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
}

auto PhysicalDevice::isDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> bool {
  bool extensions_supported = checkDeviceExtensionSupport(device);

  bool swapchain_adequate = false;
  if (extensions_supported) {
    auto swapchain_support = querySwapChainSupport(device, surface);
    swapchain_adequate = swapchain_support.IsAdequate();
  }

  auto supported_features = device.getFeatures();

  return extensions_supported && swapchain_adequate && supported_features.samplerAnisotropy == vk::True;
}

auto PhysicalDevice::scoreDevice(vk::PhysicalDevice device) -> uint32_t {
  auto device_properties = device.getProperties();

  uint32_t score = 0;

  if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
    score += 1000;
  } else if (device_properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
    score += 10;
  }

  score += device_properties.limits.maxImageDimension2D;

  return score;
}

} // namespace rendy::graphics::vulkan
