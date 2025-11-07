#pragma once
#include "../math/Vec2.hpp"
#include "../ecs/Registry.hpp"
#include "../display/DisplayConfig.hpp"
#include "../game/utils/Assets.hpp"
class EntityFactory {
public:
    explicit EntityFactory(Registry* registry, DisplayConfig* display, Assets* assets)
        : m_registry(registry)
        , m_display(display)
        , m_assets(assets)
    {
    }

    Entity createBackground();
    Entity createPlayer();
    Entity createBallShadow(const Vec2& pos, const Vec2& vel);
    Entity createBall(const Vec2& pos, const Vec2& vel, float height);

    float debugBallHeight = 0.f;
    float debugBallVelocity = 0.f;
    bool clickToSpawn = true;

private:
    Registry* m_registry;
    DisplayConfig* m_display;
    Assets* m_assets;
};
