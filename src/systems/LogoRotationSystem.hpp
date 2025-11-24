#pragma once

#include "../game/utils/GameContext.hpp"
#include "../components/Components.hpp"
#include "../ecs/Component.hpp"
class LogoRotationSystem {
public:
	void update(GameContext* context, float dt);
};