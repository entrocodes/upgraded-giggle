add_executable(PixelPong
    main.cpp
    game/Game.cpp
    game/EntityFactory.cpp
    ecs/Registry.cpp
    systems/RenderSystem.cpp
    systems/PlayerInputSystem.cpp
    systems/PlayerActionSystem.cpp
    systems/GridDebugSystem.cpp
    systems/MovementSystem.cpp
    systems/BoundarySystem.cpp
    imgui/ImGuiLayer.cpp
)

target_include_directories(PixelPong PRIVATE src)

target_link_libraries(PixelPong
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)

add_custom_command(TARGET PixelPong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PixelPong>/assets
)