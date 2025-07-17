#include "renderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

constexpr int kWidth = 800;
constexpr int kHeight = 600;

auto main() -> int {
  spdlog::set_level(spdlog::level::level_enum::trace);
  spdlog::info("Starting Rendy...");

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    spdlog::error("SDL could not initialize! SDL_ERROR: {}", SDL_GetError());
    return 1;
  }

  auto *sdl_window = SDL_CreateWindow("Vulkan Engine", kWidth, kHeight, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
  if (sdl_window == nullptr) {
    spdlog::error("Window could not be created! SDL_Error: {}", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  auto renderer = rendy::renderer::vulkan::Renderer();
  renderer.Initialize();

  bool quit_app = false;
  while (!quit_app) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
          quit_app = true;
        }
      }
    }
  }

  renderer.Destroy();

  SDL_DestroyWindow(sdl_window);
  SDL_Quit();

  spdlog::info("Rendy Shutting Down...");
  return 0;
}
