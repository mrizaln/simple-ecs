# simple-ecs
Simple ECS implementation in C++23 based on [an article by Austin Morlan](https://austinmorlan.com/posts/entity_component_system/)

## Preview

https://github.com/user-attachments/assets/dc7e4db3-37ee-4aaa-9a44-2841d3661aa7

## Building

This project uses conan to install the dependency, make sure it is installed and configured correctly before building this project.

```sh
conan install . --build missing -s build_type=Release
cmake --preset conan-release           # or conan-default if in windows
cmake --build --preset conan-release
./build/Release/nexus
```
