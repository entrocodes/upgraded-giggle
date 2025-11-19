add_executable(PixelPong
    main.cpp
    debug/Debug.cpp
    game/GameEngine.cpp
    game/scenes/GameScene.cpp
    game/EntityFactory.cpp
    game/utils/Assets.cpp
    game/utils/Animation.cpp
    systems/BallForceSystem.cpp
    systems/RenderSystem.cpp
    systems/InputSystem.cpp
    systems/PlayerInputSystem.cpp
    systems/MetaInputSystem.cpp
    systems/PlayerActionSystem.cpp
    systems/GridDebugSystem.cpp
    systems/MovementSystem.cpp
    systems/BoundarySystem.cpp
    systems/BallRemovalSystem.cpp
    systems/AnimationSystem.cpp
    imgui/ImGuiLayer.cpp
    math/TableHomography.cpp
    systems/RenderLayerSystem.cpp
    systems/NetCollisionSystem.cpp
    systems/BallMovementSystem.cpp
)

target_compile_features(PixelPong PRIVATE cxx_std_17)


target_include_directories(PixelPong PRIVATE src)
target_link_libraries(PixelPong PRIVATE Eigen3::Eigen)

target_link_libraries(PixelPong
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
add_custom_command(TARGET PixelPong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/bin $<TARGET_FILE_DIR:PixelPong>/bin
)