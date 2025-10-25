// EntityFactory.hpp
#pragma once
#include "../ecs/Registry.hpp"

namespace EntityFactory {
    Entity createPlayer(Registry& registry);
    Entity createEnemy(Registry& registry);
}
