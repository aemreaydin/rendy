add_executable(${PROJECT_NAME} main.cpp)

#Link the main executable against the shared libraries
#Note : For hot - reloading, you typically don't link directly to the hot-reloadable
#modules(like game_logic) at compile time.Instead, you load them dynamically.
#However, core engine components might be linked directly if they are
#always present and not intended for hot - reloading themselves.
#For this example, engine_core and renderer_vulkan are linked directly,
#while game_logic is designed for dynamic loading(though not fully implemented
#in this basic main.cpp).
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        # common
        # engine_core
        rendy_vulkan
        spdlog::spdlog
        SDL3::SDL3 # Link SDL3 from Conan
        imgui::imgui # Link ImGui from Conan
        Vulkan::Vulkan # Link Vulkan
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/modules/renderer/include
    # ${CMAKE_SOURCE_DIR}/modules/common/include
    # ${CMAKE_SOURCE_DIR}/modules/engine_core/include
    # ${CMAKE_SOURCE_DIR}/modules/game_logic/include
)
