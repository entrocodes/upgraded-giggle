#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"
#include "../imgui/ImGuiLayer.hpp"

GameEngine::GameEngine()
{
    context.window.create(sf::VideoMode(context.display.windowSize.x, context.display.windowSize.y), "PixelPong");
    context.assets.loadFromFile("bin/assets.txt");
    m_sceneManager.registerScene<GameScene>("game", &context);
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    context.window.setVerticalSyncEnabled(true);

    sf::Clock deltaClock;
    ImGui::SFML::Init(context.window);

    while (context.window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        sf::Event event;

        while (context.window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event); // forward events to ImGui

            switch (event.type) {
            case sf::Event::Closed:
                context.window.close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::F11)
                    toggleFullscreen();
                break;

            case sf::Event::Resized:
                handleResize(event.size.width, event.size.height);

                break;

            default:
                break;
            }
        }

        // --- Update display configuration ---
        context.display.updateFromWindow(context.window);

        // --- Update ImGui + game scene ---

        ImGui::SFML::Update(context.window, dt);
        m_sceneManager.handleInput(&context);
        m_sceneManager.update(context.window, &context, dt);

        // --- Render ---
        context.window.clear();
        m_sceneManager.render(context.window, &context);
        ImGui::SFML::Render(context.window);
        context.window.display();
    }

    ImGui::SFML::Shutdown();
}

const Assets& GameEngine::assets() const
{
    return context.assets;
}
sf::RenderWindow& GameEngine::window() {
    return context.window;
}

void GameEngine::handleResize(float width, float height) {
    context.display.windowSize = { width, height };
    context.display.updateFromWindow(context.window);
    ImGui::GetIO().DisplaySize = ImVec2(width, height);

}

void GameEngine::toggleFullscreen() {
    ImGui::SFML::Shutdown();
    context.display.fullscreen = !context.display.fullscreen;
    context.window.close();

    if (context.display.fullscreen)
        context.window.create(sf::VideoMode::getDesktopMode(), "PixelPong", sf::Style::Fullscreen);
    else
        context.window.create(sf::VideoMode(context.display.windowSize.x, context.display.windowSize.y), "PixelPong");

    ImGui::SFML::Init(context.window);

    context.display.updateFromWindow(context.window);
}


