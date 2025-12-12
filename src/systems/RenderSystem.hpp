#pragma once
#include <SFML/Graphics.hpp>
#include "../game/utils/GameContext.hpp"
#include "../debug/systems/Display3DBoundingBoxes.hpp"
#include "../ecs/system/ISystem.hpp"
struct RenderSystem  : public ISystem{

public:
    SystemExec update(GameContext* context);
private:
    Display3DBoundingBoxes display3DBoundingBoxes;
    void drawBallLogo(GameContext* context, CBall* ballComp, CTransform* transform);
    void drawText(GameContext* context, CText* text, CTransform* transform)
};
