#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
#include "../math/physics/forces/CalculateBallGravity.hpp"
#include "../math/physics/forces/CalculateMagnusForce.hpp"
#include "../math/physics/forces/CalculateBallDrag.hpp"
struct BallForces {
    Vec3 forceGravity = { 0.0f , 0.0f , 0.0f };
    Vec3 forceMagnus = { 0.0f , 0.0f , 0.0f };
    Vec3 forceDrag = { 0.0f , 0.0f , 0.0f };
    Vec3 totalForces = { 0.0f , 0.0f , 0.0f };
    Vec3 acceleration = { 0.0f , 0.0f , 0.0f };
};
class BallForceSystem {
public:
    CalculateBallDrag calcBallDrag;
    CalculateBallGravity calcBallGrav;
    CalculateMagnusForce calcMagnus;
    void update(GameContext* context, float dt);
};
