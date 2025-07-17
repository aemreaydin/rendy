#include "renderer.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <span>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace rendy::renderer::vulkan {

void Renderer::Initialize() {

  uint32_t sdl_instance_extension_count{0};
  const auto *sdl_instance_extensions =
      SDL_Vulkan_GetInstanceExtensions(&sdl_instance_extension_count);
  const auto sdl_instance_extensions_span =
      std::span{sdl_instance_extensions, sdl_instance_extension_count};
  spdlog::info("Required instance extensions by SDL: {}",
               fmt::join(sdl_instance_extensions_span, ", "));

  _instance = std::make_unique<Instance>();
  if (!_instance->Initialize(sdl_instance_extensions_span)) {
    throw std::runtime_error("Failed to create Vulkan instance.");
  }
}

} // namespace rendy::renderer::vulkan
