# simple-ecs
Simple ECS implementation in C++23 based on [an article by Austin Morlan](https://austinmorlan.com/posts/entity_component_system/)

## Preview

https://github.com/user-attachments/assets/9203cc33-cd01-4ad8-9c92-e7e7c7ab80ad

## Building

This project uses conan to install the dependency, make sure it is installed and configured correctly before building this project.

```sh
conan install . --build missing -s build_type=Release
cmake --preset conan-release           # or conan-default if in windows
cmake --build --preset conan-release
./build/Release/nexus
```
