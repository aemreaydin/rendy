#pragma once

#include "enums.hpp"
#include "rendy_api_export.h"

namespace rendy::graphics::core {

struct DeviceCapabilities {
  bool compute_support{false};
};

class RENDY_API Device {
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
