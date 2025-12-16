#include "GameEngine.hpp"
#include <imgui.h>         
#include <imgui-SFML.h>  
#include "scenes/GameScene.hpp"
#include "scenes/MenuScene.hpp"
#include "../imgui/ImGuiLayer.hpp"
#include "../display/DisplayUtils.hpp"

GameEngine::GameEngine() {
    context.window.create(
        sf::VideoMode(context.display.windowSize.x, context.display.windowSize.y),
        "PixelPong"
    );

    context.assets.loadFromFile("bin/assets.txt");
    context.sceneManager.registerScene<GameScene>("game", &context);
    context.sceneManager.registerScene<MenuScene>("menu", &context);
    context.sceneManager.switchTo("game");
}

void GameEngine::run() {

    context.window.setVerticalSyncEnabled(true);
    context.window.setFramerateLimit(0);

    float accumulator = 0.f;
    sf::Clock clock;

    ImGui::SFML::Init(context.window);

    while (context.window.isOpen()) {

        sf::Time frameTime = clock.restart();
        float realDt = frameTime.asSeconds();

        if (realDt > MAX_ACCUM_TIME)
            realDt = MAX_ACCUM_TIME;

        accumulator += realDt;
        if (accumulator > MAX_ACCUM_TIME)
            accumulator = MAX_ACCUM_TIME;

        context.frameStats.dt = realDt;

        context.rawInput.nextFrame();

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

        if (context.renderSettings.updateResolution || context.display.toggleFullscreen) {
            context.display.isFullscreen = !context.display.isFullscreen;
            updateResolution();
            context.display.toggleFullscreen = false;
        }

        context.display.updateFromWindow(context.window);
        DisplayUtils::applyLetterboxedView(&context);

        ImGui::SFML::Update(context.window, frameTime);

        while (accumulator >= FIXED_DT) {
            context.frameStats.dt = FIXED_DT;
            context.frameStats.frameIndex++;
            context.sceneManager.update();
            accumulator -= FIXED_DT;
        }
        if (context.metaInputState.returnToMainMenu) {
            context.metaInputState.returnToMainMenu = false ;
            context.sceneManager.switchTo("menu"); //THIS WILL BE IMPROVED UPON SOON, WITH A SCENEREQUEST SYSTEM

        }
        if (context.metaInputState.quit) {
            context.window.close();
        }
        
        context.frameAlpha = accumulator / FIXED_DT;
        context.frameStats.dt = realDt;

        context.window.clear();
        context.sceneManager.render();
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

    display.updateFromWindow(context.window);
    DisplayUtils::applyLetterboxedView(&context);
    ImGui::GetIO().DisplaySize = ImVec2(display.windowSize.x, display.windowSize.y);
}
