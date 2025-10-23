#include "MovementSystem.hpp"
#include "../ecs/Registry.hpp"
#include "../components/Transform.hpp"
#include "../components/Velocity.hpp"

void MovementSystem::update(Registry& registry, float dt) {
    for (auto e : registry.getEntitiesWith<Transform, Velocity>()) {
        auto* transform = registry.getComponent<Transform>(e);
        auto* velocity = registry.getComponent<Velocity>(e);
        if (transform && velocity)
            transform->position += velocity->velocity * dt;
    }

}

