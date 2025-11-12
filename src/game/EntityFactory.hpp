#pragma once
#include "../math/Vec3.hpp"
#include "../ecs/Registry.hpp"
#include "../display/DisplayConfig.hpp"
#include "../game/utils/Assets.hpp"
#include "../game/utils/Camera.hpp"
class EntityFactory {
public:
    explicit EntityFactory(Registry& registry, DisplayConfig& display, Assets& assets, Camera& camera)
        : m_registry(registry)
        , m_display(display)
        , m_assets(assets)
        , m_camera(camera)
    {
    }

    Entity createBackground();
    Entity createPlayer();
    Entity createBallShadow(const Vec2& pos);
    Entity createBall(const Vec3& pos, const Vec3& vel);


private:
    Registry& m_registry;
    DisplayConfig& m_display;
    Assets& m_assets;
    Camera& m_camera;
};
