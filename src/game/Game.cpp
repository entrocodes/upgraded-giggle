#include "Game.hpp"
#include "../ecs/Registry.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/MovementSystem.hpp"
#include "../systems/PlayerInputSystem.hpp"
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

    // create player
    Entity player = registry.createEntity();
    auto& transformPlayer = registry.addComponent<Transform>(player);
    transformPlayer.position = Grid::toWorldCentered(1,1);
    transformPlayer.rotation = 0.f;
    registry.addComponent<Velocity>(player);
    registry.addComponent<Player>(player);
    auto& spritePlayer = registry.addComponent<Sprite>(player);
    auto texturePlayer = std::make_shared<sf::Texture>();
    texturePlayer->loadFromFile("assets/ball.png");
    spritePlayer.setTexture(texturePlayer);
    registry.addComponent<BoundingBox>(player, spritePlayer.sprite.getLocalBounds());

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
        playerInput.update(registry, dt.asSeconds());
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
