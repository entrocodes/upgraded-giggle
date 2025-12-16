add_executable(PixelPong
    main.cpp
    debug/Debug.cpp
    game/GameEngine.cpp
    game/scenes/GameScene.cpp
    game/scenes/MenuScene.cpp
    game/EntityFactory.cpp
    game/utils/Assets.cpp
    game/utils/Animation.cpp
    systems/TextButtonSystem.cpp
    systems/BallForceSystem.cpp
    systems/RenderSystem.cpp
    systems/InputSystem.cpp
    systems/game/intent/PlayerIntentSystem.cpp
    systems/game/intent/GlobalIntentSystem.cpp
    systems/game/intent/debug/DebugIntentSystem.cpp
    systems/game/action/PlayerActionSystem.cpp
    systems/game/action/GlobalActionSystem.cpp
    systems/game/action/debug/DebugActionSystem.cpp
    systems/menu/action/ActionSystem.cpp
    systems/menu/intent/IntentSystem.cpp
    systems/GridDebugSystem.cpp
    systems/BoundarySystem.cpp
    systems/BallRemovalSystem.cpp
    systems/AnimationSystem.cpp
    systems/game/imgui/ImGuiLayer.cpp
    math/TableHomography.cpp
    systems/RenderLayerSystem.cpp
    systems/NetCollisionSystem.cpp
    systems/game/racket/RacketCollisionSystem.cpp
    systems/game/racket/RacketSwingSystem.cpp
    systems/game/racket/RacketHandleSystem.cpp
    systems/game/racket/RacketTransformSystem.cpp
    systems/game/racket/RacketArmSystem.cpp
    systems/BallIntegrateSystem.cpp
    systems/LogoRotationSystem.cpp
    systems/DirtyTextSystem.cpp
    ecs/system/SystemGraph.cpp
    game/scenes/Scene.cpp
    helpers/FileUtils.cpp
    helpers/JoystickUtils.cpp
    game/utils/JsonEntityLoader.cpp
)
if (MSVC)
    target_compile_options(PixelPong PRIVATE /FS)
endif()

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