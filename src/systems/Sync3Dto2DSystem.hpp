#pragma once

#include "../ecs/system/ISystem.hpp"
#include "../game/utils/GameContext.hpp"
#include <string>
class Sync3Dto2DSystem : public ISystem {
public:
	SystemExec update(GameContext* context);
};