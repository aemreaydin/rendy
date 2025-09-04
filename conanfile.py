import os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout

class RendyEngineConan(ConanFile):
    name = "rendy_engine"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    description = "A 3D Vulkan Engine with hot-reloadable modules."
    url = "https://github.com/aemreaydin/rendy"
    license = "MIT"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": True, 
        "fPIC": True, 
        "glfw/*:shared": True,
        "imgui/*:shared": True, 
    }

    requires = (
        "glfw/3.4",
        "imgui/1.92.0-docking", 
        "magic_enum/0.9.7",
        "nlohmann_json/3.12.0",
        "yaml-cpp/0.8.0",
        "spdlog/1.15.3"
    )

    generators = "CMakeDeps", "CMakeToolchain"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def layout(self):
        self.folders.build_folder_vars = ["settings.os", "settings.build_type"]
        self.folders.generators = "generators"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        pass
