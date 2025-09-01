#include "vulkan/renderer.hpp"
#include "vulkan/device.hpp"
#include "vulkan/instance.hpp"

#include <memory>
#include <span>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

namespace rendy::graphics::vulkan {

void Renderer::Initialize(SDL_Window &window) {
  uint32_t sdl_instance_extension_count{0};
  const auto *sdl_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_instance_extension_count);
  const auto sdl_instance_extensions_span = std::span{sdl_instance_extensions, sdl_instance_extension_count};

  _instance = std::make_unique<Instance>();
  if (!_instance->Initialize(sdl_instance_extensions_span)) {
    throw std::runtime_error("Failed to create Vulkan instance.");
  }
  VkSurfaceKHR surface{};
  if (!SDL_Vulkan_CreateSurface(&window, _instance->Get(), nullptr, &surface)) {
    throw std::runtime_error("Failed to create Vulkan surface.");
  }
  _surface = std::make_unique<vk::SurfaceKHR>(surface);
  _physical_device = std::make_unique<PhysicalDevice>();
  if (!_physical_device->Initialize(*_instance, *_surface)) {
    throw std::runtime_error("Failed to choose a valid Vulkan physical device.");
  }

  _device = std::make_unique<VulkanDevice>(_physical_device);
  if (!_device->Initialize()) {
    throw std::runtime_error("Failed to create Vulkan device");
  }
}

void Renderer::Destroy() {
  _instance->Get().destroySurfaceKHR(*_surface);
  _instance->Destroy();
}

} // namespace rendy::graphics::vulkan
