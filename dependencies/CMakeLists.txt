include(FetchContent)

# ==============================================================================
# 1. THE SETTINGS (Must come FIRST)
# ==============================================================================

# This is the "Magic Flag" - it forces ALL dependencies to be static (.lib)
# This will stop SFML and ImGui from ever looking for a .dll
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build all dependencies as static" FORCE)

# Specific ImGui-SFML tweaks
set(IMGUI_SFML_SHARED_LIB OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
set(IMGUI_SFML_IMGUI_DEMO ON CACHE BOOL "" FORCE)

# SFML tweaks
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

# ==============================================================================
# 2. THE DECLARATIONS
# ==============================================================================
set(SFML_VERSION 2.6.1)
set(IMGUI_VERSION 1.89)

FetchContent_Declare(SFML URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip")
FetchContent_Declare(imgui URL "https://github.com/ocornut/imgui/archive/v${IMGUI_VERSION}.zip")
FetchContent_Declare(imgui-sfml GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git GIT_TAG 2.6.x)
FetchContent_Declare(Eigen GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git GIT_TAG 3.4.0)
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3)

# ==============================================================================
# 3. THE POPULATION (The "Go" Button)
# ==============================================================================

# First, get SFML, Eigen, and the raw ImGui source
FetchContent_MakeAvailable(SFML Eigen nlohmann_json imgui)

# IMPORTANT: Tell ImGui-SFML where the ImGui headers are BEFORE making it available
set(IMGUI_DIR ${imgui_SOURCE_DIR} CACHE PATH "Path to imgui" FORCE)

# Finally, build the wrapper
FetchContent_MakeAvailable(imgui-sfml)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EVENTS ON CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE) 
set(SDL_AUDIO OFF CACHE BOOL "" FORCE) # SFML handles Audio (if needed)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.x # Solid version for GameController support
)

FetchContent_MakeAvailable(SDL2)