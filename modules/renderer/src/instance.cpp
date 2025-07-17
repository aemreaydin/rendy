#include "instance.hpp"
#include <algorithm>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vector>

VKAPI_ATTR static auto VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                void * /*p_user_data*/) -> VkBool32 {
  auto message_type_to_string = [](VkDebugUtilsMessageTypeFlagsEXT type) -> std::string {
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
      return "General";
    }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
      return "Validation";
    }
    if (type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
      return "Performance";
    }
    return "Unknown";
  };
  switch (message_severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    spdlog::debug("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    spdlog::info("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    spdlog::warn("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    spdlog::error("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  default:
    spdlog::trace("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  }
  return VK_FALSE;
}
namespace rendy::renderer::vulkan {

#ifdef RENDY_DEBUG
constexpr bool kRendyDebug = true;
#else
constexpr bool kRendyDebug = false;
#endif

constexpr const char *kAppName = "Rendy";
constexpr const char *kEngineName = "Rendy Engine";

constexpr const char *kPortabilityExtension = "VK_KHR_portability_enumeration";
constexpr const char *kDebugUtilsExtension = "VK_EXT_debug_utils";

constexpr const char *kValidationLayer = "VK_LAYER_KHRONOS_validation";

auto Instance::Initialize(std::span<const char *const> window_extensions) -> bool {

  const VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                      .pNext = nullptr,
                                      .pApplicationName = kAppName,
                                      .pEngineName = kEngineName,
                                      .apiVersion = VK_MAKE_VERSION(1, 4, 0)};

  std::vector<const char *> required_extensions;
  required_extensions.assign(window_extensions.begin(), window_extensions.end());

  std::vector<const char *> required_layers;
  void *p_next = nullptr;
  if (kRendyDebug) {
    spdlog::info("This is a debug build. Validation Layers will be enabled.");
    required_extensions.emplace_back(kDebugUtilsExtension);
    required_layers.emplace_back(kValidationLayer);
    createDebugUtilsMessengerCreateInfo();
    p_next = &_vk_debug_utils_messenger_create_info;
  }

  if (!validateExtensions(required_extensions) || !validateLayers(required_layers)) {
    spdlog::error("Some of the extensions and/or layers are not supported by this device.");
    return false;
  }

  spdlog::info("Enabling Extensions({})", required_extensions.size());
  spdlog::info("{}", fmt::join(required_extensions, ", "));
  if (!required_layers.empty()) {
    spdlog::info("Enabling Layers({})", required_layers.size());
    spdlog::info("{}", fmt::join(required_layers, ", "));
  }

  VkInstanceCreateInfo instance_create_info{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = p_next,
#ifdef __APPLE__
      .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
#endif
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
      .ppEnabledLayerNames = required_layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
      .ppEnabledExtensionNames = required_extensions.data(),
  };

  auto result = vkCreateInstance(&instance_create_info, nullptr, &_vk_instance);
  if (result != VK_SUCCESS) {
    spdlog::error("Failed to create Vulkan instance({}).", magic_enum::enum_name(result));
    return false;
  }

  if (kRendyDebug) {
    const auto result = initializeDebugUtilsMessenger();
    if (result != VK_SUCCESS) {
      spdlog::error("Failed to create debug messenger({})", magic_enum::enum_name(result));
      return false;
    }
    spdlog::info("Vulkan validation messenger created.");
  }

  spdlog::info("Vulkan instance created.");
  return true;
}

void Instance::Destroy() {
  if (kRendyDebug) {
    _fn_destroy_debug_utils_messenger(_vk_instance, _vk_debug_utils_messenger, nullptr);
  }
  vkDestroyInstance(_vk_instance, nullptr);
  spdlog::info("Vulkan instance destroyed.");
}

void Instance::createDebugUtilsMessengerCreateInfo() {
  _vk_debug_utils_messenger_create_info = {.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                                           .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                                           .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                           .pfnUserCallback = DebugCallback,
                                           .pUserData = nullptr};
}

auto Instance::initializeDebugUtilsMessenger() -> VkResult {
  _fn_create_debug_utils_messenger =
      PFN_vkCreateDebugUtilsMessengerEXT(vkGetInstanceProcAddr(_vk_instance, "vkCreateDebugUtilsMessengerEXT"));
  _fn_destroy_debug_utils_messenger =
      PFN_vkDestroyDebugUtilsMessengerEXT(vkGetInstanceProcAddr(_vk_instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (_fn_create_debug_utils_messenger == nullptr || _fn_destroy_debug_utils_messenger == nullptr) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return _fn_create_debug_utils_messenger(_vk_instance, &_vk_debug_utils_messenger_create_info, nullptr,
                                          &_vk_debug_utils_messenger);
}

void Instance::destroyDebugUtilsMessenger() {
  _fn_destroy_debug_utils_messenger(_vk_instance, _vk_debug_utils_messenger, nullptr);
}

auto Instance::validateExtensions(const std::vector<const char *> &required_extensions) -> bool {
  uint32_t available_extensions_count{0};
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(available_extensions_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &available_extensions_count, available_extensions.data());

  return std::ranges::all_of(required_extensions, [&](const char *extension_name) {
    auto iter = std::ranges::find(available_extensions, std::string_view(extension_name),
                                  &VkExtensionProperties::extensionName);
    if (iter == available_extensions.end()) {
      spdlog::warn("Extension {} not supported by this device.", iter->extensionName);
      return false;
    }
    return true;
  });
}

auto Instance::validateLayers(const std::vector<const char *> &required_layers) -> bool {
  uint32_t available_layers_count{0};
  vkEnumerateInstanceLayerProperties(&available_layers_count, nullptr);
  std::vector<VkLayerProperties> available_layers(available_layers_count);
  vkEnumerateInstanceLayerProperties(&available_layers_count, available_layers.data());

  return std::ranges::all_of(required_layers, [&](const char *layer_name) {
    auto iter = std::ranges::find(available_layers, std::string_view(layer_name), &VkLayerProperties::layerName);
    if (iter == available_layers.end()) {
      spdlog::warn("Layer {} not supported by this device.", iter->layerName);
      return false;
    }
    return true;
  });
}

auto Instance::Get() const -> VkInstance { return _vk_instance; }

} // namespace rendy::renderer::vulkan
