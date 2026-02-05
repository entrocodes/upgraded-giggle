#pragma once
#include "ecs/Component.hpp"
#include "game/pose/Pose.hpp"
class CPose : public Component
{
public:
	Pose pose;

	CPose() {}
};