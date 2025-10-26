// EntityFactory.hpp
#pragma once
#include "../ecs/Registry.hpp"
#include "../math/Vec2.hpp"
namespace EntityFactory {
    Entity createPlayer(Registry& registry);
    Entity createEnemy(Registry& registry, const Vec2& pos, const Vec2& vel);
}
