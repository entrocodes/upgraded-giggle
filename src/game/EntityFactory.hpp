#pragma once
#include "../ecs/Registry.hpp"
#include "../math/Vec2.hpp"
#include "GameEngine.hpp"

class EntityFactory {
public:
    EntityFactory() = default;
    explicit EntityFactory(GameEngine* gameEngine);

    Entity createPlayer(Registry& registry);
    Entity createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel);

private:
    GameEngine* m_game = nullptr;
};
