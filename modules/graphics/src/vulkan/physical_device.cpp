#include "vulkan/physical_device.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/utils.hpp"
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

auto PhysicalDevice::Initialize(Instance &instance) -> vk::Bool32 {
  auto physical_devices =
      VkCheckAndUnwrap(instance.Get().enumeratePhysicalDevices(), "Failed to enumerate physical devices");

  if (physical_devices.empty()) {
    spdlog::error("Couldn't find a physical device.");
    return vk::False;
  }

  _vk_physical_device = physical_devices[0];
  queryDeviceInfo();
  return vk::True;
}

void PhysicalDevice::Destroy() {}

auto PhysicalDevice::Get() const -> vk::PhysicalDevice { return _vk_physical_device; }

void PhysicalDevice::queryDeviceInfo() {
  _vk_features = _vk_physical_device.getFeatures();
  _vk_properties = _vk_physical_device.getProperties();
  _vk_memory_properties = _vk_physical_device.getMemoryProperties();
  _vk_queue_family_properties = _vk_physical_device.getQueueFamilyProperties();
}

} // namespace rendy::graphics::vulkan
