#include "GameEngine.hpp"
#include "scenes/GameScene.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../display/DisplayUtils.hpp"
GameEngine::GameEngine() {
    context.window.create(
        sf::VideoMode(context.display.windowSize.x, context.display.windowSize.y),
        "PixelPong"
    );

    context.assets.loadFromFile("bin/assets.txt");
    m_sceneManager.registerScene<GameScene>("game", &context);
    m_sceneManager.switchTo("game");
}

void GameEngine::run() {
    context.window.setVerticalSyncEnabled(true);
    sf::Clock deltaClock;
    ImGui::SFML::Init(context.window);

    while (context.window.isOpen()) {
        // --- Begin frame ---
        context.rawInput.nextFrame(); // update previous input state
        sf::Time dt = deltaClock.restart();
        sf::Event event;

        // --- Event polling ---
        while (context.window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type) {
            case sf::Event::Closed:
                context.window.close();
                break;

            case sf::Event::KeyPressed:
                context.rawInput.keyStates[event.key.code] = true;
                if (event.key.code == sf::Keyboard::F11 || event.key.code == sf::Keyboard::F)
                    context.display.fullscreen = !context.display.fullscreen;
                break;

            case sf::Event::KeyReleased:
                context.rawInput.keyStates[event.key.code] = false;
                break;

            case sf::Event::MouseButtonPressed:
                context.rawInput.mouseButtonStates[event.mouseButton.button] = true;
                break;

            case sf::Event::MouseButtonReleased:
                context.rawInput.mouseButtonStates[event.mouseButton.button] = false;
                break;

            case sf::Event::MouseMoved:
                context.rawInput.mousePosition = {
                    static_cast<float>(event.mouseMove.x),
                    static_cast<float>(event.mouseMove.y)
                };
                break;

            case sf::Event::Resized:
                handleResize(event.size.width, event.size.height);
                break;

            default:
                break;
            }
        }

        if (context.renderSettings.updateResolution || context.display.fullscreen) {
            updateResolution();
        }
        // --- Update display configuration ---
        context.display.updateFromWindow(context.window);

        // Apply logical scaling / letterboxing
        DisplayUtils::applyLetterboxedView(&context);

        // --- Update game + ImGui ---
        ImGui::SFML::Update(context.window, dt);
        m_sceneManager.handleInput();
        m_sceneManager.update(dt);


        // --- Render ---
        context.window.clear();
        m_sceneManager.render();
        ImGui::SFML::Render(context.window);
        context.window.display();
    }

    ImGui::SFML::Shutdown();
}

void GameEngine::handleResize(float width, float height) {
    context.display.windowSize = { width, height };
    context.display.updateFromWindow(context.window);
    ImGui::GetIO().DisplaySize = ImVec2(width, height);
}

void GameEngine::updateResolution() {
    auto& rs = context.renderSettings;
    auto& display = context.display;

    auto newRes = rs.resolutions[rs.currentResolutionIndex].second;

    rs.updateResolution = false;

    context.window.create(
        sf::VideoMode((unsigned)newRes.x, (unsigned)newRes.y),
        "PixelPong",
        display.fullscreen ? sf::Style::Fullscreen : sf::Style::Default
    );

    // Update display info from new window
    display.updateFromWindow(context.window);

    // Apply logical scaling / letterboxing
    DisplayUtils::applyLetterboxedView(&context);

    // Fix ImGui interaction scaling
    ImGui::GetIO().DisplaySize = ImVec2(display.windowSize.x, display.windowSize.y);
}
