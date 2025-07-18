add_library(rendy_vulkan SHARED src/renderer.cpp src/instance.cpp)

target_compile_definitions(
    rendy_vulkan
    PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_EXCEPTIONS
)
target_compile_definitions(
    rendy_vulkan
    PRIVATE $<$<CONFIG:Debug>:RENDY_VK_DEBUG>
)
target_include_directories(
    rendy_vulkan
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(
    rendy_vulkan
    PRIVATE
        magic_enum::magic_enum
        SDL3::SDL3
        imgui::imgui
        Vulkan::Vulkan
        spdlog::spdlog
)
