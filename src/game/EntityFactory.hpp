#pragma once
#include "../ecs/Registry.hpp"
#include "../math/Vec2.hpp"
#include "../display/DisplayConfig.hpp"
#include "../game/utils/GameContext.hpp"

class EntityFactory {
public:
    explicit EntityFactory(GameContext* context = nullptr)
        : m_context(context) {
    }

    Entity createBackground(Registry& registry, DisplayConfig& display);
    Entity createPlayer(Registry& registry, DisplayConfig& display);
    Entity createBallShadow(Registry& registry, const Vec2& pos, const Vec2& vel, DisplayConfig& display);
    Entity createBall(Registry& registry, const Vec2& pos, const Vec2& vel, float height, DisplayConfig& display);

    float debugBallHeight = 0.f;
    float debugBallVelocity = 0.f;
    bool clickToSpawn = true;

private:
    GameContext* m_context;
};
