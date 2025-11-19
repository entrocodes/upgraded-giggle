#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
#include "../math/physics/forces/CalculateBallGravity.hpp"
#include "../math/physics/forces/CalculateMagnusForce.hpp"
#include "../math/physics/forces/CalculateBallDrag.hpp"
#include "../systems/NetCollisionSystem.hpp"
#include "../math/physics/forces/ApplyFriction.hpp"

class BallForceSystem {
public:
    CalculateBallDrag calcBallDrag;
    CalculateBallGravity calcBallGrav;
    CalculateMagnusForce calcMagnus;

    NetCollisionSystem netCollision;
    ApplyFriction applyFriction;

    void update(GameContext* context, float dt);
};
