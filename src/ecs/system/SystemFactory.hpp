#pragma once

#include <memory>
#include <type_traits>
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystem.hpp"
class SystemFactory {
public:
    explicit SystemFactory(GameContext* context)
        : m_context(context) {
    }

    template<typename T, typename... Args>
    std::unique_ptr<ISystem> create(Args&&... args) {
        static_assert(
            std::is_base_of_v<ISystem, T>,
            "SystemFactory::create<T>: T must derive from ISystem"
            );

        // Explicit upcast through constructor ?safe and correct
        return std::unique_ptr<ISystem>(
            new T(std::forward<Args>(args)...)
        );
    }

private:
    GameContext* m_context;
};