if(APPLE)
    cmake_minimum_required(VERSION 3.21)
else()
    cmake_minimum_required(VERSION 3.24)
endif()

set(CMAKE_CXX_STANDARD 20)
set(VCPKG_ROOT "vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
project(betterblox)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm QUIET)
find_package(GLM QUIET)

add_executable(betterblox src/Biome.hpp src/Block.hpp src/Camera.hpp src/Inventory.hpp src/main.cpp src/perlin.hpp src/PerlinNoise.hpp src/Player.hpp src/Shader.hpp src/stb_image.h src/BetterBlox.hpp src/ChunkLoader.hpp src/utils/RuntimeError.hpp)
target_link_libraries(betterblox PRIVATE glfw glad::glad glm::glm)

# Copies assets to build dir.
add_custom_target(assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(betterblox assets)