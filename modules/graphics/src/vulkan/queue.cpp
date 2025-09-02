#include "vulkan/queue.hpp"
#include "vulkan/utils.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace rendy::graphics::vulkan {

void QueueRegistry::RegisterFamily(uint32_t family_index, core::QueueType types, uint32_t count) {
  _families[family_index] =
      QueueFamilyInfo{.family_index = family_index, .supported_types = types, .queue_count = count};

  std::vector<std::string> type_names;
  if (static_cast<uint32_t>(types & core::QueueType::Graphics) != 0) {
    type_names.emplace_back("Graphics");
  }
  if (static_cast<uint32_t>(types & core::QueueType::Compute) != 0) {
    type_names.emplace_back("Compute");
  }
  if (static_cast<uint32_t>(types & core::QueueType::Transfer) != 0) {
    type_names.emplace_back("Transfer");
  }

  spdlog::info("Registered queue family {}: {} queues, supports [{}]", family_index, count,
               fmt::join(type_names, ", "));
}

auto QueueRegistry::GetQueueCreateInfos() const -> std::vector<vk::DeviceQueueCreateInfo> {
  std::vector<vk::DeviceQueueCreateInfo> create_infos(_families.size());

  for (const auto &[family_idx, info] : _families) {
    create_infos.emplace_back(vk::DeviceQueueCreateInfo{.queueFamilyIndex = family_idx,
                                                        .queueCount = 1, // Request 1 queue per family for now
                                                        .pQueuePriorities = _queue_priorities.data()});
  }

  spdlog::info("Creating {} unique queue families", create_infos.size());
  return create_infos;
}

auto QueueRegistry::GetFamilyFor(core::QueueType type) const -> uint32_t {
  uint32_t best_family = 0;
  uint32_t best_capability_count = UINT32_MAX;
  bool found_any = false;

  for (const auto &[family_idx, info] : _families) {
    // Check if this family supports the requested type
    if (static_cast<uint32_t>(info.supported_types & type) != 0) {
      if (!found_any) {
        // First supported family found - use as fallback
        best_family = family_idx;
        found_any = true;
      }

      // Count how many capabilities this family has (fewer = more dedicated)
      uint32_t capability_count = 0;
      auto types_val = static_cast<uint32_t>(info.supported_types);

      if ((types_val & static_cast<uint32_t>(core::QueueType::Graphics)) != 0U) {
        capability_count++;
      }
      if ((types_val & static_cast<uint32_t>(core::QueueType::Compute)) != 0U) {
        capability_count++;
      }
      if ((types_val & static_cast<uint32_t>(core::QueueType::Transfer)) != 0U) {
        capability_count++;
      }

      // Prefer queues with fewer capabilities (more dedicated)
      if (capability_count < best_capability_count) {
        best_family = family_idx;
        best_capability_count = capability_count;
      }
    }
  }

  if (!found_any) {
    // spdlog::error("No queue family found for requested type");
    return _families.begin()->first; // Return first available family as last resort
  }

  return best_family;
}

auto QueueRegistry::GetAllFamilies() const -> const std::map<uint32_t, QueueFamilyInfo> & { return _families; }

auto FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) -> QueueFamilyIndices {
  auto queue_families = device.getQueueFamilyProperties();

  QueueFamilyIndices indices{};
  bool found_graphics_with_present = false;

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    const auto &queue_family = queue_families[i];

    // Graphics with presentation support (required)
    if (!found_graphics_with_present && (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)) {
      auto present_support = VkCheckAndUnwrap(device.getSurfaceSupportKHR(i, surface), "Failed to get surface support");
      if (present_support != vk::False) {
        indices.graphics_family = i;
        found_graphics_with_present = true;
      }
    }

    // Compute (optional)
    if (!indices.compute_family.has_value() && (queue_family.queueFlags & vk::QueueFlagBits::eCompute)) {
      indices.compute_family = i;
    }

    // Transfer (optional) - prefer dedicated transfer queues
    if (queue_family.queueFlags & vk::QueueFlagBits::eTransfer) {
      // Prefer dedicated transfer queues (transfer-only)
      bool is_dedicated = !(queue_family.queueFlags & vk::QueueFlagBits::eGraphics) &&
                          !(queue_family.queueFlags & vk::QueueFlagBits::eCompute);

      if (is_dedicated || !indices.transfer_family.has_value()) {
        indices.transfer_family = i;
      }
    }
  }

  if (!found_graphics_with_present) {
    spdlog::error("No graphics queue family with presentation support found");
  }

  return indices;
}

} // namespace rendy::graphics::vulkan
