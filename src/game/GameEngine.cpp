#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"

GameEngine::GameEngine()
    : m_window(sf::VideoMode(640, 640), "PixelPong ECS")
{
    m_sceneManager.registerScene<GameScene>("game");
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    sf::Clock deltaClock;

    while (m_window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();

        m_sceneManager.handleInput(m_window, dt);
        m_sceneManager.update(m_window, dt);

        m_window.clear();
        m_sceneManager.render(m_window);
        m_window.display();
    }
}
