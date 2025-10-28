#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"
#include <imgui-SFML.h>

GameEngine::GameEngine()
    : m_window(sf::VideoMode(640, 640), "PixelPong ECS")
{
    ImGui::SFML::Init(m_window);

    m_sceneManager.registerScene<GameScene>("game");
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    sf::Clock deltaClock;

    while (m_window.isOpen()) {
        context.deltaTime = deltaClock.restart().asSeconds();
        context.windowSize = { float(m_window.getSize().x), float(m_window.getSize().y) };

        m_sceneManager.handleInput(m_window, context);
        m_sceneManager.update(context);

        m_window.clear();
        m_sceneManager.render(m_window);
        m_sceneManager.renderImGui();

        ImGui::SFML::Render(m_window);
        m_window.display();
    }

    ImGui::SFML::Shutdown();
}
