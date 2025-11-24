#pragma once
#include "../game/utils/GameContext.hpp"
class AnimationSystem {
public:
	void update(GameContext* context);
private:
	void updatePlayer(GameContext* context);

};