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
    systems/game/intent/PlayerIntentSystem.cpp
    systems/game/intent/GlobalIntentSystem.cpp
    systems/game/intent/debug/DebugIntentSystem.cpp
    systems/game/action/PlayerActionSystem.cpp
    systems/game/action/GlobalActionSystem.cpp
    systems/game/action/debug/DebugActionSystem.cpp
    systems/GridDebugSystem.cpp
    systems/BoundarySystem.cpp
    systems/BallRemovalSystem.cpp
    systems/AnimationSystem.cpp
    imgui/ImGuiLayer.cpp
    math/TableHomography.cpp
    systems/RenderLayerSystem.cpp
    systems/NetCollisionSystem.cpp
    systems/RacketCollisionSystem.cpp
    systems/RacketSwingSystem.cpp
    systems/RacketHandleSystem.cpp
    systems/RacketTransformSystem.cpp
    systems/RacketArmSystem.cpp
    systems/BallIntegrateSystem.cpp
    systems/LogoRotationSystem.cpp
    ecs/system/SystemGraph.cpp
    game/scenes/Scene.cpp
    helpers/FileUtils.cpp
    game/utils/JsonEntityLoader.cpp
)

target_compile_features(PixelPong PRIVATE cxx_std_17)


target_include_directories(PixelPong PRIVATE src)
target_link_libraries(PixelPong PRIVATE Eigen3::Eigen)

target_link_libraries(PixelPong
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

target_link_libraries(PixelPong PRIVATE nlohmann_json::nlohmann_json)

include(Install.cmake)
add_custom_command(TARGET PixelPong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/bin $<TARGET_FILE_DIR:PixelPong>/bin
)