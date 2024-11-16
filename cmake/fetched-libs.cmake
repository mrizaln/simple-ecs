set(FETCHCONTENT_QUIET FALSE)

include(FetchContent)

# glfw-cpp requires glfw version 3.3 above
FetchContent_Declare(
  glfw-cpp
  GIT_REPOSITORY https://github.com/mrizaln/glfw-cpp
  GIT_TAG v0.7.0)
FetchContent_MakeAvailable(glfw-cpp)

add_library(fetch::glfw-cpp ALIAS glfw-cpp)
