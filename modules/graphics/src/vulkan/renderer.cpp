#include "vulkan/renderer.hpp"
#include "vulkan/device.hpp"

#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <span>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

void Renderer::Initialize() {
  uint32_t sdl_instance_extension_count{0};
  const auto *sdl_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_instance_extension_count);
  const auto sdl_instance_extensions_span = std::span{sdl_instance_extensions, sdl_instance_extension_count};

  _instance = std::make_unique<Instance>();
  if (!_instance->Initialize(sdl_instance_extensions_span)) {
    throw std::runtime_error("Failed to create Vulkan instance.");
  }
  // auto device = graphics::vulkan::VulkanDevice{_instance.get()};
}

void Renderer::Destroy() { _instance->Destroy(); }

} // namespace rendy::graphics::vulkan
