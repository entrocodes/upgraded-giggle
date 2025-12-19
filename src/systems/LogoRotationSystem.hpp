#pragma once

#include "game/utils/GameContext.hpp"
#include "components/Components.hpp"
#include "ecs/Component.hpp"
#include "ecs/system/ISystem.hpp"
class LogoRotationSystem : public ISystem {
public:
	SystemExec update(GameContext* context);
};