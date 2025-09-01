#pragma once

#include "core/device.hpp"
#include "vulkan/queue.hpp"
#include <map>
#include <memory>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {
class PhysicalDevice;
class VulkanDevice final : public core::Device {
  vk::Device _device;
  core::DeviceCapabilities _device_capabilities{};
  std::shared_ptr<PhysicalDevice> _physical_device;
  QueueRegistry _queue_registry;

  // Queue handles mapped by type
  std::map<core::QueueType, vk::Queue> _queues;

public:
  VulkanDevice(std::shared_ptr<PhysicalDevice> physical_device);

  auto GetGraphicsAPI() -> core::GraphicsAPI override;
  auto Initialize() -> bool override;
  void Cleanup() override;

  // Queue access
  [[nodiscard]] auto GetQueue(core::QueueType type) const -> vk::Queue;
  [[nodiscard]] auto GetQueueRegistry() const -> const QueueRegistry & { return _queue_registry; }
};

} // namespace rendy::graphics::vulkan
