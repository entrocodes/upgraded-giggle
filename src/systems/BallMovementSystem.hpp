#pragma once
#include <SFML/System.hpp> // for sf::Time or deltaTime
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../math/Vec3.hpp"
#include "../math/Vec2.hpp"
#include "../systems/NetCollisionSystem.hpp"
#include "../systems/BallForceSystem.hpp"
#include "../ecs/Entity.hpp"
class BallMovementSystem {
public:
    void update(GameContext* context, float dt);
private:
    BallForceSystem ballForceSystem;
    NetCollisionSystem netCollisionSystem;
    void updateOffTable(GameContext* context);
    void handleTableContact(GameContext* context, Entity& entity, float dt);
    void handleFloorContact(GameContext* context, Entity& entity, float dt);
};
