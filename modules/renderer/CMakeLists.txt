add_library(rendy_vulkan SHARED src/renderer.cpp src/instance.cpp)
target_include_directories(rendy_vulkan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(rendy_vulkan
    PRIVATE
        magic_enum::magic_enum
        SDL3::SDL3
        imgui::imgui
        Vulkan::Vulkan
        spdlog::spdlog
)
