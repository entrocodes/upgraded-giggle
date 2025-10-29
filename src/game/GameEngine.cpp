#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"
#include "../imgui/ImGuiLayer.hpp"
GameEngine::GameEngine()
    : m_window(sf::VideoMode(640, 640), "PixelPong ECS")
{
    m_assets.loadFromFile("bin/assets.txt");
    m_sceneManager.registerScene<GameScene>("game");
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    sf::Clock deltaClock;

    ImGui::SFML::Init(m_window);
    while (m_window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        sf::Event event;
        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event); // forward to ImGui
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        m_sceneManager.handleInput(m_window);
        m_sceneManager.update(m_window, dt);
        m_window.clear();
        m_sceneManager.render(m_window);
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}
