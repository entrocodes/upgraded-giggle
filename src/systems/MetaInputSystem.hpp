#pragma once
#include "../input/RawInputState.hpp"

struct MetaInputState {
    bool quit = false;
    bool paused = false;
};

class MetaInputSystem {
public:
    // Updates the meta state based on the current raw input
    void update(const RawInputState& rawInput, MetaInputState& state);
};
