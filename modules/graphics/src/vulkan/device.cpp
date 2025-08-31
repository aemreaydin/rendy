#include "vulkan/device.hpp"
#include "vulkan/physical_device.hpp"
#include "vulkan/utils.hpp"

namespace rendy::graphics::vulkan {

VulkanDevice::VulkanDevice(const PhysicalDevice& physical_device) {
  const vk::DeviceCreateInfo device_create_info{};
  _device = VkCheckAndUnwrap(physical_device.Get().createDevice(device_create_info), "Failed to create device.");
}

auto VulkanDevice::GetGraphicsAPI() -> core::GraphicsAPI { return core::GraphicsAPI::Vulkan; }
void VulkanDevice::Initialize() {}
void VulkanDevice::Cleanup() {}

} // namespace rendy::graphics::vulkan