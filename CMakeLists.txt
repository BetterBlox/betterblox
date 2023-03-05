cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(VCPKG_ROOT "vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(betterblox)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm QUIET)
find_package(GLM QUIET)

add_executable(betterblox src/biome.hpp src/Block.hpp src/camera.hpp src/glad.c src/inventory.hpp src/main.cpp src/perlin.hpp src/PerlinNoise.hpp src/Player.hpp src/shader.hpp src/stb_image.h)
target_link_libraries(betterblox PRIVATE glfw glad::glad glm::glm)

# Copies assets to build dir.
add_custom_target(assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(betterblox assets)