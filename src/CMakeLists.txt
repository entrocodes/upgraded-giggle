
# 1. Setup Project Sources 
file(GLOB_RECURSE PIXEL_PONG_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(PixelPong ${PIXEL_PONG_SOURCES})


# 4. Include Paths
target_include_directories(PixelPong PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${SDL2_INCLUDE_DIRS}
)

# 5. Linking (Dynamic/Shared)
# Note: Using the standard targets provided by the packages
target_link_libraries(PixelPong 
    PRIVATE 
        sfml-graphics
        sfml-window
        sfml-system
        SDL2-static      # Standard dynamic target
        Eigen3::Eigen
        nlohmann_json::nlohmann_json
        ImGui-SFML::ImGui-SFML
)

# 6. Asset Management
add_custom_command(TARGET PixelPong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/bin" "$<TARGET_FILE_DIR:PixelPong>/bin"
)