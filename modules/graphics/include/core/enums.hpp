#pragma once

#include <cstdint>
namespace rendy::graphics::core {

enum class GraphicsAPI : uint8_t { Vulkan, DirectX12, Metal, WebGPU };

enum class QueueType : uint8_t {
  Graphics = 0x1,
  Compute = 0x2,
  Transfer = 0x4,
};

inline auto operator|(QueueType lhs, QueueType rhs) -> QueueType {
  return static_cast<QueueType>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline auto operator&(QueueType lhs, QueueType rhs) -> QueueType {
  return static_cast<QueueType>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

} // namespace rendy::graphics::core
