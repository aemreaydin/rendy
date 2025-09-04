add_library(
    rendy_graphics
    SHARED
    src/core/device.cpp
    src/core/buffer.cpp
    src/core/image.cpp
    src/core/texture.cpp
    src/core/pipeline.cpp
    src/core/command_list.cpp
    src/vulkan/queue.cpp
    src/vulkan/device.cpp
    src/vulkan/instance.cpp
    src/vulkan/physical_device.cpp
    src/vulkan/renderer.cpp
)

set_target_properties(
    rendy_graphics
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        OUTPUT_NAME "rendy_graphics"
)

target_include_directories(
    rendy_graphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(
    rendy_graphics
    PRIVATE
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_CONSTRUCTORS
        VULKAN_HPP_NO_EXCEPTIONS
        GLFW_INCLUDE_VULKAN
)
if(MSVC)
    target_compile_definitions(rendy_graphics PRIVATE RENDY_GRAPHICS_EXPORTS)
endif()

target_compile_definitions(
    rendy_graphics
    PRIVATE $<$<CONFIG:Debug>:RENDY_VK_DEBUG>
)

# Link dependencies
target_link_libraries(
    rendy_graphics
    PUBLIC
    PRIVATE Vulkan::Vulkan spdlog::spdlog glfw
)

if(MSVC)
    target_compile_options(rendy_graphics PRIVATE /W4)
else()
    target_compile_options(rendy_graphics PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install the library
install(TARGETS rendy_graphics
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
