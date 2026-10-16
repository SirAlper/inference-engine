cmake_minimum_required(VERSION 3.20)
project(InferenceEngine VERSION 0.1.0 LANGUAGES CXX)

# ---------------------------------------------------------
#  Language standard
# ---------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)          # plain c++20 instead of gnu++20

# Generate compile_commands.json for IntelliSense / clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if no build type is given
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------
#  Options (override from the command line, e.g. -DENGINE_ENABLE_AVX2=OFF)
# ---------------------------------------------------------
option(ENGINE_BUILD_TESTS      "Build unit tests"            ON)
option(ENGINE_BUILD_BENCHMARKS "Build benchmarks"            ON)
option(ENGINE_ENABLE_AVX2      "Use AVX2 and FMA instructions" ON)

# ---------------------------------------------------------
#  Common warnings
# ---------------------------------------------------------
# INTERFACE library: not compiled itself, only carries settings.
# Linked only to our own targets, so protobuf-generated code and
# third-party libraries are not flooded with warnings.
add_library(engine_warnings INTERFACE)
target_compile_options(engine_warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor)

# ---------------------------------------------------------
#  Dependencies
# ---------------------------------------------------------
# CONFIG mode: uses the CMake files installed by protobuf itself.
# Also resolves the abseil dependency of newer protobuf versions.
find_package(Protobuf CONFIG REQUIRED)

# ---------------------------------------------------------
#  Subdirectories
# ---------------------------------------------------------
# TODO: enable once proto/onnx.proto is added (ONNX loader step)
# add_subdirectory(proto)
add_subdirectory(src)

if(ENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(ENGINE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
