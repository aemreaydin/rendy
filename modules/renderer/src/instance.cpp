#include "instance.hpp"
#include "utils.hpp"
#include <algorithm>
#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

VKAPI_ATTR static auto VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                vk::DebugUtilsMessageTypeFlagsEXT message_type,
                                                vk::DebugUtilsMessengerCallbackDataEXT const *callback_data,
                                                void * /*p_user_data*/) -> vk::Bool32 {
  auto message_type_to_string = [](vk::DebugUtilsMessageTypeFlagsEXT type) -> std::string {
    if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral) {
      return "General";
    }
    if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) {
      return "Validation";
    }
    if (type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance) {
      return "Performance";
    }
    return "Unknown";
  };
  switch (message_severity) {
  case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
    spdlog::debug("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
    spdlog::info("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
    spdlog::warn("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
    spdlog::error("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  default:
    spdlog::trace("[Vulkan {}] {}", message_type_to_string(message_type), callback_data->pMessage);
    break;
  }
  return vk::False;
}
namespace rendy::renderer::vulkan {

#ifdef RENDY_VK_DEBUG
constexpr bool kRendyDebug = true;
#else
constexpr bool kRendyDebug = false;
#endif

constexpr const char *kAppName = "Rendy";
constexpr const char *kEngineName = "Rendy Engine";

constexpr const char *kPortabilityExtension = "VK_KHR_portability_enumeration";
constexpr const char *kDebugUtilsExtension = "VK_EXT_debug_utils";

constexpr const char *kValidationLayer = "VK_LAYER_KHRONOS_validation";

auto Instance::Initialize(std::span<const char *const> window_extensions) -> vk::Bool32 {
  VULKAN_HPP_DEFAULT_DISPATCHER.init();

  uint32_t vk_version{};
  if (const auto version_result = vk::enumerateInstanceVersion(&vk_version); version_result != vk::Result::eSuccess) {
    spdlog::error("Failed to enumerate instance version.");
    return vk::False;
  }
  spdlog::info("Vulkan Instance Version: {}.{}.{}", vk::apiVersionMajor(vk_version), vk::apiVersionMinor(vk_version),
               vk::apiVersionPatch(vk_version));

  _vk_api_version = vk::makeApiVersion(0, 1, 4, 0);

  if (vk_version < _vk_api_version) {
    spdlog::error("Vulkan instance doesn't support requested version.");
    spdlog::error("Requested Version: {}.{}.{}", vk::apiVersionMajor(vk_version), vk::apiVersionMinor(vk_version),
                  vk::apiVersionPatch(vk_version));
    return vk::False;
  }

  const vk::ApplicationInfo app_info = {
      .pApplicationName = kAppName, .pEngineName = kEngineName, .apiVersion = _vk_api_version};

  std::vector<const char *> required_extensions;
  required_extensions.assign(window_extensions.begin(), window_extensions.end());

  std::vector<const char *> required_layers;
  void *p_next = nullptr;
  if (kRendyDebug) {
    spdlog::info("This is a debug build. Validation Layers are enabled.");
    required_extensions.emplace_back(kDebugUtilsExtension);
    required_layers.emplace_back(kValidationLayer);
    createDebugUtilsMessengerCreateInfo();
    p_next = &_vk_debug_utils_messenger_create_info;
  }

  if (!validateExtensions(required_extensions) || !validateLayers(required_layers)) {
    spdlog::error("Some of the extensions and/or layers are not supported by this device.");
    return vk::False;
  }

  spdlog::info("Enabling Extensions({}): {}", required_extensions.size(), fmt::join(required_extensions, ", "));
  if (!required_layers.empty()) {
    spdlog::info("Enabling Layers({}): {}", required_layers.size(), fmt::join(required_layers, ", "));
  }

  vk::InstanceCreateInfo const instance_create_info{
      .pNext = p_next,
#ifdef __APPLE__
      .flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
#endif
      .pApplicationInfo = &app_info,
      .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
      .ppEnabledLayerNames = required_layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(required_extensions.size()),
      .ppEnabledExtensionNames = required_extensions.data(),
  };

  _vk_instance = VkCheckAndUnwrap(vk::createInstance(instance_create_info, nullptr), "Failed to create instance.");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(_vk_instance);
  spdlog::info("Vulkan instance created.");

  if (kRendyDebug) {
    if (const auto debug_messenger_result = initializeDebugUtilsMessenger(); debug_messenger_result == vk::False) {
      return vk::False;
    }
  }
  spdlog::info("Vulkan validation messenger created.");
  return vk::True;
}

void Instance::Destroy() const {
  if (kRendyDebug) {
    _vk_instance.destroyDebugUtilsMessengerEXT(_vk_debug_utils_messenger);
  }
  vkDestroyInstance(_vk_instance, nullptr);
  spdlog::info("Vulkan instance destroyed.");
}

void Instance::createDebugUtilsMessengerCreateInfo() {
  _vk_debug_utils_messenger_create_info =
      vk::DebugUtilsMessengerCreateInfoEXT{.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                                              vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                              vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
                                           .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                          vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                          vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                           .pfnUserCallback = &DebugCallback,
                                           .pUserData = nullptr};
}

auto Instance::initializeDebugUtilsMessenger() -> vk::Bool32 {
  _vk_debug_utils_messenger =
      VkCheckAndUnwrap(_vk_instance.createDebugUtilsMessengerEXT(_vk_debug_utils_messenger_create_info),
                       "Failed to create DebugUtilsMessengerEXT");
  return vk::True;
}

auto Instance::validateExtensions(const std::vector<const char *> &required_extensions) -> bool {
  auto available_extensions = VkCheckAndUnwrap(vk::enumerateInstanceExtensionProperties(),
                                               "Failed to enumerate instance extension properties.");

  return std::ranges::all_of(required_extensions, [&](const char *extension_name) {
    const auto iter = std::ranges::find(available_extensions, std::string_view(extension_name),
                                        &VkExtensionProperties::extensionName);
    if (iter == available_extensions.end()) {
      spdlog::warn("Extension {} not supported by this device.", iter->extensionName);
      return false;
    }
    return true;
  });
}

auto Instance::validateLayers(const std::vector<const char *> &required_layers) -> bool {
  auto available_layers =
      VkCheckAndUnwrap(vk::enumerateInstanceLayerProperties(), "Failed to enumerate instance layer properties");

  return std::ranges::all_of(required_layers, [&](const char *layer_name) {
    auto iter = std::ranges::find(available_layers, std::string_view(layer_name), &VkLayerProperties::layerName);
    if (iter == available_layers.end()) {
      spdlog::warn("Layer {} not supported by this device.", iter->layerName);
      return false;
    }
    return true;
  });
}

auto Instance::Get() const -> vk::Instance { return _vk_instance; }

} // namespace rendy::renderer::vulkan
