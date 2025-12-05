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
    // Let the driver handle FPS via vsync for now
    context.window.setVerticalSyncEnabled(true);
    context.window.setFramerateLimit(0);

    float accumulator = 0.f;
    sf::Clock clock;          // measures real frame-to-frame time

    ImGui::SFML::Init(context.window);

    while (context.window.isOpen()) {
        // --- Measure time since last frame ---
        sf::Time frameTime = clock.restart();
        float dt = frameTime.asSeconds();
        
        // Clamp absurd spikes (e.g. minimize/restore)
        if (dt > MAX_ACCUM_TIME)
            dt = MAX_ACCUM_TIME;
        context.frameStats.dt = dt;
        accumulator += dt;
        if (accumulator > MAX_ACCUM_TIME)
            accumulator = MAX_ACCUM_TIME;

        // --- Raw input state (edge detection) ---
        context.rawInput.nextFrame();

        // --- Events ---
        sf::Event event;
        while (context.window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            switch (event.type) {
            case sf::Event::Closed:
                context.window.close();
                break;
            case sf::Event::KeyPressed:
                context.rawInput.keyStates[event.key.code] = true;
                if (event.key.code == sf::Keyboard::F11 || event.key.code == sf::Keyboard::F)
                    context.display.toggleFullscreen = true;
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

        // --- Resolution / fullscreen toggle ---
        if (context.renderSettings.updateResolution || context.display.toggleFullscreen) {
            context.display.isFullscreen = !context.display.isFullscreen;
            updateResolution();
            context.display.toggleFullscreen = false;
        }

        // --- Display + view ---
        context.display.updateFromWindow(context.window);
        DisplayUtils::applyLetterboxedView(&context);
        // Handle Input
        m_sceneManager.handleInput();
        // --- ImGui update with REAL dt (not fixed dt) ---
        ImGui::SFML::Update(context.window, frameTime);

        // --- Fixed-step game update ---
        while (accumulator >= FIXED_DT) {
            m_sceneManager.update(sf::seconds(FIXED_DT));
            accumulator -= FIXED_DT;
        }

        // Interpolation factor for rendering
        context.frameAlpha = accumulator / FIXED_DT;

        // --- Render ---
        context.window.clear();
        m_sceneManager.render();           // uses frameAlpha for lerp if you wired it
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
        display.isFullscreen ? sf::Style::Fullscreen : sf::Style::Default
    );

    // Update display info from new window
    display.updateFromWindow(context.window);

    // Apply logical scaling / letterboxing
    DisplayUtils::applyLetterboxedView(&context);

    // Fix ImGui interaction scaling
    ImGui::GetIO().DisplaySize = ImVec2(display.windowSize.x, display.windowSize.y);
}
