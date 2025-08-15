#pragma once

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

namespace rendy::renderer::vulkan {

template <typename T>
[[nodiscard]] inline auto VkCheckAndUnwrap(const vk::ResultValue<T>& result_value, std::string_view error_message) -> T {
  if (result_value.result != vk::Result::eSuccess) {
    throw std::runtime_error(std::string(error_message) + " | " + vk::to_string(result_value.result));
  }
  return result_value.value;
}

} // namespace rendy::renderer::vulkan
