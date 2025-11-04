#pragma once
#include "../ecs/Registry.hpp"
#include "../math/Vec2.hpp"
#include "../display/DisplayConfig.hpp"
class GameEngine; // forward

class EntityFactory {
public:
    explicit EntityFactory(GameEngine* gameEngine = nullptr);

    // create entities into the given registry
    Entity createBackground(Registry& registry, DisplayConfig& display);
    Entity createPlayer(Registry& registry, DisplayConfig& display);
    Entity createBallShadow(Registry& registry, const Vec2& pos, const Vec2& vel, DisplayConfig& display);
    Entity createBall(Registry& registry, const Vec2& pos, const Vec2& vel, const float height, DisplayConfig& display);

    float debugBallHeight = 0.f;
    float debugBallVelocity = 0.f;
    bool clickToSpawn = true;
private:
    GameEngine* m_game;
};
