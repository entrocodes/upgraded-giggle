#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
struct BallForces {
    Vec3 forceGravity = { 0.0f , 0.0f , 0.0f };
    Vec3 forceMagnus = { 0.0f , 0.0f , 0.0f };
    Vec3 forceDrag = { 0.0f , 0.0f , 0.0f };
    Vec3 totalForce = { 0.0f , 0.0f , 0.0f };
    Vec3 acceleration = { 0.0f , 0.0f , 0.0f }
}
class BallForceSystem {
public:
    const float pixelsPerMeter = 90; // tune this to match table/asset scale
    const float tableBottom = 504.f;
    void update(GameContext* context, float dt);
};
