# 1. Setup Project Sources
set(PIXEL_PONG_SOURCES
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
    systems/game/imgui/GameImGuiSystem.cpp
    systems/menu/imgui/MenuImGuiSystem.cpp
    math/TableHomography.cpp
    systems/RenderLayerSystem.cpp
    systems/NetCollisionSystem.cpp
    systems/game/racket/RacketCollisionSystem.cpp
    systems/game/racket/RacketSwingSystem.cpp
    
    systems/game/racket/RacketBoundingBoxSystem.cpp
    systems/game/racket/RacketArmSystem.cpp
    systems/BallIntegrateSystem.cpp
    systems/LogoRotationSystem.cpp
    systems/DirtyTextSystem.cpp
    systems/game/debug/BallSpawnDebugSystem.cpp
    ecs/system/SystemGraph.cpp
    game/scenes/Scene.cpp
    helpers/FileUtils.cpp
    game/utils/JsonEntityLoader.cpp
    input/RawInputState.cpp
    systems/game/movement/player/footwork/FootworkMovementSystem.cpp
    systems/game/movement/player/VelocityIntegrationSystem.cpp
    systems/ProceduralAnimationSystem.cpp
    systems/game/movement/anatomy/AnatomySystem.cpp
    systems/game/racket/RacketOrientationSystem.cpp
    systems/game/imgui/GameImGuiConsole.cpp
)



# 3. Create Executable
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