#pragma once

#include "enums.hpp"

namespace rendy::graphics::core {

struct DeviceCapabilities {
  bool compute_support{false};
};

class Device {
public:
  Device() = default;
  Device(const Device &) = delete;
  Device(Device &&) = delete;
  auto operator=(const Device &) -> Device & = delete;
  auto operator=(Device &&) -> Device & = delete;
  virtual ~Device() = default;

  virtual auto GetGraphicsAPI() -> GraphicsAPI = 0;

  virtual auto Initialize() -> bool = 0;
  virtual void Cleanup() = 0;
};

} // namespace rendy::graphics::core
