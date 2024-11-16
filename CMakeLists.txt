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

add_executable(main source/main.cpp)
target_include_directories(main PRIVATE source)
target_link_libraries(main PRIVATE fmt::fmt glm::glm fetch::glfw-cpp)
target_compile_options(main PRIVATE -Wall -Wextra -Wconversion -Wswitch-enum)

# sanitizer
target_compile_options(main PRIVATE -fsanitize=address,leak,undefined)
target_link_options(main PRIVATE -fsanitize=address,leak,undefined)
