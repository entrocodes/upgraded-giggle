#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "../game/scenes/Scene.hpp"

class SceneManager {
public:
    template <typename T, typename... Args>
    void registerScene(const std::string& name, Args&&... args) {
        m_scenes[name] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void switchTo(const std::string& name) {
        auto it = m_scenes.find(name);
        if (it != m_scenes.end()) {
            m_current = it->second.get();
        }
    }

    void handleInput(sf::RenderWindow& window, DisplayConfig& display) {
        if (m_current) m_current->handleInput(window, display);
    }

    void update(sf::RenderWindow& window, DisplayConfig& display, sf::Time dt) {
        if (m_current) m_current->update(window, display, dt);
    }

    void render(sf::RenderWindow& window, DisplayConfig& display) {
        if (m_current) m_current->render(window, display);
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    Scene* m_current = nullptr;
};
