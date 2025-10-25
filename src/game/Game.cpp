#include "Game.hpp"
#include "EntityFactory.hpp"
#include "../ecs/Registry.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
#include "../systems/PlayerActionSystem.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"
#include "../components/Sprite.hpp"
#include "../components/Player.hpp"
#include "../components/BoundingBox.hpp"
#include "../math/GridTransform.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

void Game::run() {
    sf::RenderWindow window(sf::VideoMode(640, 640), "PixelPong ECS");
    sf::Clock deltaClock;

    ImGuiLayer imgui;
    imgui.init(window);

    Registry registry;
    RenderSystem renderer;
    MovementSystem movement;
    PlayerInputSystem playerInput;
    PlayerActionSystem playerAction;
    Entity player = EntityFactory::createPlayer(registry);


    // -------------------------
    // Main game loop
    // -------------------------
    while (window.isOpen()) {
        sf::Event event;
        sf::Time dt = deltaClock.restart();

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) window.close();
        }

        // Update ImGui
        imgui.update(window, dt);

        // Update ECS systems
        playerInput.update(registry);
        playerAction.update(registry, dt.asSeconds());
        movement.update(registry, window, dt.asSeconds());
        // Render
        window.clear();
        renderer.render(window, registry);

        imgui.render();
        ImGui::SFML::Render(window);

        window.display();
    }

    imgui.shutdown();
}
