#pragma once
#include <SFML/Graphics.hpp>
#include "game/utils/GameContext.hpp"
#include "debug/systems/Display3DBoundingBoxes.hpp"
#include "ecs/system/ISystem.hpp"
struct RenderSystem  : public ISystem{

public:
    SystemExec update(GameContext* context);
private:
    Display3DBoundingBoxes display3DBoundingBoxes;
    void drawBallLogo(GameContext* context, CBall* cBall, CTransform* cBallTransform, CTransform3D* cBallTransform3D);
    void sync3Dto2D(GameContext* context, Entity e, CTransform* cTransform, CTransform3D* cTransform3D , bool isShadow = false);
};
