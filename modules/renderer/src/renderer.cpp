#include "renderer.hpp"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <span>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

namespace rendy::renderer::vulkan {

void Renderer::Initialize() {
  uint32_t sdl_instance_extension_count{0};
  const auto *sdl_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_instance_extension_count);
  const auto sdl_instance_extensions_span = std::span{sdl_instance_extensions, sdl_instance_extension_count};

  _instance = std::make_unique<Instance>();
  if (_instance->Initialize(sdl_instance_extensions_span) == vk::False) {
    throw std::runtime_error("Failed to create Vulkan instance.");
  }
}

void Renderer::Destroy() { _instance->Destroy(); }

} // namespace rendy::renderer::vulkan
