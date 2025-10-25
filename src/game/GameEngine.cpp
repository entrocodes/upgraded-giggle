#include "GameEngine.hpp"
#include "Game.hpp"
#include "../ecs/Registry.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
#include "../systems/PlayerActionSystem.hpp"
#include "../systems/InputSystem.hpp"
#include "../systems/MetaInputSystem.hpp"
#include "../input/RawInputState.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include <SFML/Graphics.hpp>

GameEngine::GameEngine()
    : window(sf::VideoMode(640, 640), "PixelPong ECS")
{
}

void GameEngine::run() {
    // ----------------------------------
    // Initialization
    // ----------------------------------
    ImGuiLayer imgui;
    imgui.init(window);

    Registry registry;
    RenderSystem renderer;
    MovementSystem movement;
    InputSystem inputSystem;
    PlayerInputSystem playerInput;
    PlayerActionSystem playerAction;
    MetaInputSystem metaInput;
    RawInputState rawInput;
    MetaInputState metaState;

    Game game(registry);
    game.init();  // Explicit initialization

    sf::Clock deltaClock;

    // ----------------------------------
    // Main loop
    // ----------------------------------
    while (window.isOpen()) {
        sf::Time dt = deltaClock.restart();

        // 1️⃣ Capture raw input
        inputSystem.update(window, rawInput);

        // 2️⃣ Handle meta input (quit, pause, etc.)
        metaInput.update(rawInput, metaState);
        if (metaState.quit) {
            window.close();
            break;
        }

        // 3️⃣ Update player input and actions
        playerInput.update(registry, rawInput);
        playerAction.update(registry, dt.asSeconds());
        // 4️⃣ Pause handling
        if (!metaState.paused) {
            
            game.update(window, dt.asSeconds(), movement);
        }

        // 5️⃣ ImGui + rendering
        imgui.update(window, dt);
        window.clear();
        renderer.render(window, registry);
        imgui.render();
        ImGui::SFML::Render(window);
        window.display();
    }

    imgui.shutdown();
}
