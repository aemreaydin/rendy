cmake_minimum_required(VERSION 3.30)
project(rendy CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(SANITIZE_ADDRESS "Enable AddressSanitizer (ASan)" OFF)
option(SANITIZE_UNDEFINED "Enable UndefinedBehaviorSanitizer (UBSan)" OFF)
option(SANITIZE_LEAK "Enable LeakSanitizer (LSan)" OFF)
option(SANITIZE_THREAD "Enable ThreadSanitizer (TSan)" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(SANITIZE_ADDRESS)
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
        message(STATUS "AddressSanitizer (ASan) Enabled.")
    endif()
    if(SANITIZE_UNDEFINED)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
        message(STATUS "UndefinedBehaviorSanitizer (UBSan) Enabled.")
    endif()
    if(SANITIZE_LEAK AND NOT SANITIZE_ADDRESS)
        add_compile_options(-fsanitize=leak)
        add_link_options(-fsanitize=leak)
        message(STATUS "LeakSanitizer (LSan) Enabled.")
    endif()
    if(SANITIZE_THREAD)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
        message(STATUS "ThreadSanitizer (TSan) Enabled.")
    endif()
else()
    if(
        SANITIZE_ADDRESS
        OR SANITIZE_UNDEFINED
        OR SANITIZE_LEAK
        OR SANITIZE_THREAD
    )
        message(
            WARNING
            "Sanitizers requested but not supported by current compiler (${CMAKE_CXX_COMPILER_ID})."
        )
    endif()
endif()

option(ENABLE_CLANG_TIDY "Enable Clang-Tidy analysis during build" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        # Only analyze project source files, not external libraries
        set(CMAKE_CXX_CLANG_TIDY
            ${CLANG_TIDY}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            --system-headers=false
        )
        message(STATUS "Clang-Tidy enabled (analyzing only project files).")
    else()
        message(
            WARNING
            "clang-tidy not found. Please install it to enable Clang-Tidy analysis."
        )
    endif()
endif()

option(ENABLE_IWYU "Enable Include What You Use" OFF)
if(ENABLE_IWYU)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use;-w;-Xiwyu")
endif()

find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan SDK: ${Vulkan_INCLUDE_DIRS}")
find_package(SDL3 REQUIRED)
message(STATUS "Found SDL3: ${SDL3_INCLUDE_DIRS}")
find_package(imgui REQUIRED)
message(STATUS "Found imgui: ${imgui_INCLUDE_DIRS}")
find_package(spdlog REQUIRED)
message(STATUS "Found spdlog: ${spdlog_INCLUDE_DIRS}")
find_package(magic_enum REQUIRED)
message(STATUS "Found magic_enum: ${magic_enum_INCLUDE_DIRS}")

# add_subdirectory(modules/common)
# add_subdirectory(modules/engine_core)
add_subdirectory(modules/graphics)
# add_subdirectory(modules/game_logic) # This is a hot-reloadable example
add_subdirectory(src)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
