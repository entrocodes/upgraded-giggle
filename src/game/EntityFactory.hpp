#pragma once
#include "../ecs/Registry.hpp"
#include "../math/Vec2.hpp"

class GameEngine; // forward

class EntityFactory {
public:
    explicit EntityFactory(GameEngine* gameEngine = nullptr);

    // create entities into the given registry
    Entity createBackground(Registry& registry);
    Entity createPlayer(Registry& registry);
    Entity createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel);

private:
    GameEngine* m_game;
};
