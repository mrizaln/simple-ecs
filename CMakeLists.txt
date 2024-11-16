cmake_minimum_required(VERSION 3.22)
project(simple-ecs VERSION 0.0.0)

include(cmake/prelude.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(fmt REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

include(cmake/fetched-libs.cmake) # emits: fetch::glfw-cpp

# simple-ecs library
# ~~~
add_library(simple-ecs INTERFACE)
target_include_directories(simple-ecs INTERFACE include)
# ~~~

# nexus
# ~~~
add_executable(nexus source/main.cpp source/system/physics_system.cpp)
target_include_directories(nexus PRIVATE source)
target_link_libraries(nexus PRIVATE fmt::fmt glm::glm fetch::glfw-cpp simple-ecs)
target_compile_options(nexus PRIVATE -Wall -Wextra -Wconversion -Wswitch-enum -Wno-changes-meaning)

# sanitizer
target_compile_options(nexus PRIVATE -fsanitize=address,leak,undefined)
target_link_options(nexus PRIVATE -fsanitize=address,leak,undefined)
#~~~
