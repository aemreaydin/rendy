#include "vulkan/renderer.hpp"
#include "vulkan/device.hpp"
#include "vulkan/instance.hpp"

#include <memory>
#include <span>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace rendy::graphics::vulkan {

void Renderer::Initialize(GLFWwindow &window) {
  if (glfwVulkanSupported() == GLFW_FALSE) {
    throw std::runtime_error("Glfw Vulkan support not found.");
  }

  uint32_t glfw_instance_extensions_count{};
  const auto *glfw_instance_extensions = glfwGetRequiredInstanceExtensions(&glfw_instance_extensions_count);
  const auto glfw_instance_extensions_span = std::span{glfw_instance_extensions, glfw_instance_extensions_count};

  _instance = std::make_unique<Instance>();
  if (!_instance->Initialize(glfw_instance_extensions_span)) {
    throw std::runtime_error("Failed to create Vulkan instance.");
  }
  VkSurfaceKHR surface{};
  if (glfwCreateWindowSurface(_instance->Get(), &window, nullptr, &surface) != VK_SUCCESS) {
    const char *error{};
    glfwGetError(&error);
    spdlog::error("Window could not be created! GLFW Error: {}", error);
    throw std::runtime_error("Failed to create Vulkan surface.");
  }
  _surface = std::make_unique<vk::SurfaceKHR>(surface);
  _physical_device = std::make_unique<PhysicalDevice>();
  if (!_physical_device->Initialize(*_instance, *_surface)) {
    throw std::runtime_error("Failed to choose a valid Vulkan physical device.");
  }
  spdlog::info("Selected a physical device.");

  _device = std::make_unique<VulkanDevice>(_physical_device);
  if (!_device->Initialize()) {
    throw std::runtime_error("Failed to create Vulkan device");
  }
}

void Renderer::Destroy() {
  _device->Cleanup();
  _instance->Get().destroySurfaceKHR(*_surface);
  _instance->Destroy();
}

} // namespace rendy::graphics::vulkan
