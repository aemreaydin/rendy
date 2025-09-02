#include "vulkan/renderer.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

constexpr int kWidth = 800;
constexpr int kHeight = 600;

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

auto main() -> int {
  spdlog::set_level(spdlog::level::level_enum::trace);
  spdlog::info("Starting Rendy...");

  if (glfwInit() == GLFW_FALSE) {
    const char *error{};
    glfwGetError(&error);
    spdlog::error("GLFW could not initialize! GLFW Error: {}", error);
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  auto *glfw_window = glfwCreateWindow(kWidth, kHeight, "Rendy", nullptr, nullptr);
  if (glfw_window == nullptr) {
    const char *error{};
    glfwGetError(&error);
    spdlog::error("Window could not be created! GLFW Error: {}", error);
    return 1;
  }
  glfwSetKeyCallback(glfw_window, KeyCallback);

  auto renderer = rendy::graphics::vulkan::Renderer();
  renderer.Initialize(*glfw_window);

  bool quit_app = false;
  while (glfwWindowShouldClose(glfw_window) == GLFW_FALSE) {
    glfwPollEvents();
  }

  renderer.Destroy();

  glfwDestroyWindow(glfw_window);
  glfwTerminate();

  spdlog::info("Rendy Shutting Down...");
  return 0;
}
