#pragma once
#include "SystemExec.hpp"

struct GameContext;

class ISystem {
public:
    virtual ~ISystem() = default;

    // Default behavior = Ran
    virtual SystemExec update(GameContext* context) {
        return {};
    }
};
