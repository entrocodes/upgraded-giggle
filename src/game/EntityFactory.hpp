#pragma once
#include "../math/Vec3.hpp"
#include "../ecs/Registry.hpp"
#include "../display/DisplayConfig.hpp"
#include "../game/utils/Assets.hpp"
#include "../game/utils/Camera.hpp"
#include "../game/utils/TableParameters.hpp"
class EntityFactory {
public:
    explicit EntityFactory(Registry& registry, DisplayConfig& display, Assets& assets, Camera& camera, TableParameters& tableParameters)
        : m_registry(registry)
        , m_display(display)
        , m_assets(assets)
        , m_camera(camera)
        , m_tableParameters(tableParameters)
    {
    }

    Entity createBackground();
    Entity createTable();
    Entity createNet();
    Entity createPlayer();
    Entity createBallShadow(const Vec3& shadowPos_m);
    Entity createBall(const Vec3& pos_m, const Vec3& vel_mps, const Vec3& spin = { 0.0f,0.0f,0.0f });
    Entity createPlayerRacket();

private:
    Registry& m_registry;
    DisplayConfig& m_display;
    Assets& m_assets;
    Camera& m_camera;
    TableParameters& m_tableParameters;
};
