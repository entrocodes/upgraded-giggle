#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "game/scenes/Scene.hpp"

class SceneManager {
public:
    template <typename T, typename... Args>
    void registerScene(const std::string& name, Args&&... args) {
        m_scenes[name] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    void switchTo(const std::string& name) {
        if (m_current) {
            m_current->onExit();
        }

        auto it = m_scenes.find(name);
        if (it != m_scenes.end()) {
            m_current = it->second.get();
            m_current->onEnter();
        }
    }


    void update() {
        if (m_current) m_current->update();
    }

    void render() {
        if (m_current) m_current->render();
    }

    Scene* currentScene() const {
        return m_current;
    }

    void requestSwitch(const std::string& name) {
        m_pending = name;
    }

    void applyPendingSwitch() {
        if (!m_pending.empty()) {
            switchTo(m_pending);
            m_pending.clear();
        }
    }
private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    Scene* m_current = nullptr;
    std::string m_pending;
};
