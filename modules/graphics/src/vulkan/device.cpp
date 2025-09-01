#include "vulkan/device.hpp"
#include "vulkan/physical_device.hpp"
#include "vulkan/utils.hpp"
#include <spdlog/spdlog.h>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace rendy::graphics::vulkan {

VulkanDevice::VulkanDevice(std::shared_ptr<PhysicalDevice> physical_device)
    : _physical_device(std::move(physical_device)) {}

auto VulkanDevice::GetGraphicsAPI() -> core::GraphicsAPI { return core::GraphicsAPI::Vulkan; }

auto VulkanDevice::Initialize() -> bool {
  const auto &indices = _physical_device->GetQueueFamilyIndices();
  const auto &queue_props = _physical_device->Get().getQueueFamilyProperties();

  _queue_registry.RegisterFamily(indices.graphics_family, core::QueueType::Graphics | core::QueueType::Transfer,
                                 queue_props[indices.graphics_family].queueCount);

  if (indices.compute_family.has_value()) {
    _queue_registry.RegisterFamily(indices.compute_family.value(), core::QueueType::Compute,
                                   queue_props[indices.compute_family.value()].queueCount);
    _device_capabilities.compute_support = true;
  } else {
    _device_capabilities.compute_support = false;
  }

  if (indices.transfer_family.has_value()) {
    _queue_registry.RegisterFamily(indices.transfer_family.value(), core::QueueType::Transfer,
                                   queue_props[indices.transfer_family.value()].queueCount);
  }

  // Create device with consolidated queue create infos
  auto queue_create_infos = _queue_registry.GetQueueCreateInfos();

  const std::vector<const char *> required_extensions{
#ifdef __APPLE__
      "VK_KHR_portability_subset",
#endif
      vk::KHRDynamicRenderingExtensionName, vk::KHRPushDescriptorExtensionName};

  const vk::DeviceCreateInfo device_create_info{
      .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
      .pQueueCreateInfos = queue_create_infos.data(),
      .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
      .ppEnabledExtensionNames = required_extensions.data()};

  _device = VkCheckAndUnwrap(_physical_device->Get().createDevice(device_create_info), "Failed to create device.");

  // Retrieve queue handles and populate map
  _queues[core::QueueType::Graphics] = _device.getQueue(_queue_registry.GetFamilyFor(core::QueueType::Graphics), 0);

  // Transfer queue: use dedicated if available, otherwise use graphics queue
  auto transfer_family = _queue_registry.GetFamilyFor(core::QueueType::Transfer);
  _queues[core::QueueType::Transfer] = _device.getQueue(transfer_family, 0);

  // Compute queue: only if available
  if (_device_capabilities.compute_support) {
    _queues[core::QueueType::Compute] = _device.getQueue(_queue_registry.GetFamilyFor(core::QueueType::Compute), 0);
  }

  return true;
}

auto VulkanDevice::GetQueue(core::QueueType type) const -> vk::Queue {
  auto it = _queues.find(type);
  if (it != _queues.end()) {
    return it->second;
  }

  spdlog::error("Requested queue type not available");
  return _queues.at(core::QueueType::Graphics); // Fallback to graphics
}

void VulkanDevice::Cleanup() {}

} // namespace rendy::graphics::vulkan
