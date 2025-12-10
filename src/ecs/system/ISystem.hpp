#pragma once
#include "SystemExec.hpp"

class GameContext;

class ISystem {
public:
    virtual ~ISystem() = default;

    // Default behavior = Ran
    virtual SystemExec update(GameContext* context) {
        return {};
    }
};
