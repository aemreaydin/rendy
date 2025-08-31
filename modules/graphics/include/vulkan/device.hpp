#pragma once

#include "core/device.hpp"
#include "physical_device.hpp"
#include "vulkan/vulkan.hpp"

namespace rendy::graphics::vulkan {
class Instance;
class PhysicalDevice;

class VulkanDevice final : public core::Device {
  vk::Device _device;
public:
  VulkanDevice(const PhysicalDevice& physical_device);

  auto GetGraphicsAPI() -> core::GraphicsAPI override;
  void Initialize() override;
  void Cleanup() override;
};

} // namespace rendy::graphics::vulkan
