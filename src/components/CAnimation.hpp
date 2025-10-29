#pragma once
#include "../game/utils/Animation.hpp"
class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {
	}
};