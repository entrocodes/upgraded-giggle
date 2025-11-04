#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"
#include "../imgui/ImGuiLayer.hpp"
GameEngine::GameEngine()
    : m_window(sf::VideoMode(m_display.windowSize.x, m_display.windowSize.y), "PixelPong")
{
    m_assets.loadFromFile("bin/assets.txt");
    m_sceneManager.registerScene<GameScene>("game", this, m_display);
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    sf::Clock deltaClock;
    ImGui::SFML::Init(m_window);

    while (m_window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        sf::Event event;

        while (m_window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event); // forward events to ImGui

            switch (event.type) {
            case sf::Event::Closed:
                m_window.close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::F11)
                    toggleFullscreen();
                break;

            case sf::Event::Resized:
                handleResize(event.size.width, event.size.height);

                // Optional but helpful: re-init ImGui fonts after context change
                ImGui::SFML::UpdateFontTexture();
                break;

            default:
                break;
            }
        }

        // --- Update display configuration ---
        m_display.updateFromWindow(m_window);

        // --- Update ImGui + game scene ---
        ImGui::SFML::Update(m_window, dt);
        m_sceneManager.handleInput(m_window, m_display);
        m_sceneManager.update(m_window, m_display, dt);

        // --- Render ---
        m_window.clear();
        m_sceneManager.render(m_window, m_display);
        ImGui::SFML::Render(m_window);
        m_window.display();
    }

    ImGui::SFML::Shutdown();
}

const Assets& GameEngine::assets() const
{
    return m_assets;
}
sf::RenderWindow& GameEngine::window() {
    return m_window;
}

void GameEngine::toggleFullscreen() {
    m_display.fullscreen = !m_display.fullscreen;
    m_window.close();

    if (m_display.fullscreen) {
        m_window.create(sf::VideoMode::getDesktopMode(), "PixelPong", sf::Style::Fullscreen);
    }
    else {
        m_window.create(sf::VideoMode(m_display.windowSize.x, m_display.windowSize.y), "PixelPong ECS");
    }

    ImGui::SFML::Init(m_window);
    m_display.updateFromWindow(m_window);
}

void GameEngine::handleResize(unsigned width, unsigned height) {
    m_display.windowSize = { width, height };
    m_display.updateFromWindow(m_window);
    ImGui::GetIO().DisplaySize = ImVec2(m_display.windowSize.x, m_display.windowSize.y);

    sf::View view(sf::FloatRect(0.f, 0.f, m_display.logicalSize.x, m_display.logicalSize.y));
    m_window.setView(view);
}
