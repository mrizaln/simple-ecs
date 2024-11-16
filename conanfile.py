from conan import ConanFile
from conan.tools.cmake import cmake_layout

class Recipe(ConanFile):
    settings   = ["os", "compiler", "build_type", "arch"]
    generators = ["CMakeToolchain", "CMakeDeps"]
    requires   = ["fmt/11.0.2", "glfw/3.4", "glm/1.0.1"]

    def layout(self):
        cmake_layout(self)
