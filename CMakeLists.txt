# Prevent vcpkg from overriding find_package calls
set(CMAKE_VCPKG_REPOS_OFF ON)
set(VCPKG_TARGET_TRIPLET "x64-windows-static") # Forces a specific look away from dynamic vcpkg
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
# MOVE THIS HERE: It must be set before ImGui-SFML is added/fetched
set(IMGUI_SFML_SHARED_LIB OFF CACHE BOOL "Build ImGui-SFML statically" FORCE)
cmake_minimum_required(VERSION 3.5)
project(PixelPong VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies)
add_subdirectory(src)