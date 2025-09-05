#pragma once

#include "core/enums.hpp"
#include "rendy_api_export.h"
#include <map>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

struct QueueFamilyIndices {
  uint32_t graphics_family{};              // Required (also handles presentation)
  std::optional<uint32_t> compute_family;  // Optional
  std::optional<uint32_t> transfer_family; // Optional
};

struct QueueFamilyInfo {
  uint32_t family_index{};
  uint32_t queue_count{};
  core::QueueType supported_types{};
};

class QueueRegistry {
  std::map<uint32_t, QueueFamilyInfo> _families;

public:
  void RegisterFamily(uint32_t family_index, core::QueueType types, uint32_t count);

  [[nodiscard]] auto GetQueueCreateInfos() const -> std::vector<vk::DeviceQueueCreateInfo>;
  [[nodiscard]] auto GetFamilyFor(core::QueueType type) const -> uint32_t;
  [[nodiscard]] auto GetAllFamilies() const -> const std::map<uint32_t, QueueFamilyInfo> &;

private:
  std::vector<float> _queue_priorities{1.0F};
};

// Utility functions moved from PhysicalDevice
[[nodiscard]] auto FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> QueueFamilyIndices;

} // namespace rendy::graphics::vulkan
