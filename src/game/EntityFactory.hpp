#pragma once
#include "../math/Vec2.hpp"
#include "../game/utils/GameContext.hpp"

class EntityFactory {
public:
    explicit EntityFactory(GameContext* context = nullptr)
        : m_context(context) {
    }

    Entity createBackground();
    Entity createPlayer();
    Entity createBallShadow(const Vec2& pos, const Vec2& vel);
    Entity createBall(const Vec2& pos, const Vec2& vel, float height);

    float debugBallHeight = 0.f;
    float debugBallVelocity = 0.f;
    bool clickToSpawn = true;

private:
    GameContext* m_context;
};
