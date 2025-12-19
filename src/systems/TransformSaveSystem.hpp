#pragma once

#include "ecs/system/ISystem.hpp"
#include "game/utils/GameContext.hpp"

class TransformSaveSystem : public ISystem {
public:
    SystemExec update(GameContext* context) override {
        for (auto e : context->registry.getEntitiesWith<CTransform>()) {
            auto* t = context->registry.getComponent<CTransform>(e);
            t->lastPos = t->pos;
        }
        return { SystemExecResult::Ran };
    }
};
