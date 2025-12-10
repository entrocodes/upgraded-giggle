#pragma once
#include "../ecs/system/ISystem.hpp"

class FrameStatsSystem final : public ISystem {
public:
    SystemExec update(GameContext* context) override {
        auto& fs = context->frameStats;

        fs.accumulator += fs.dt;
        fs.frames++;

        if (fs.accumulator >= 1.0f) {
            fs.fps = fs.frames / fs.accumulator;
            fs.frames = 0;
            fs.accumulator = 0.f;
        }
        return {SystemExecResult::Ran};
    }
};
