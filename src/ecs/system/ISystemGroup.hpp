// ISystemGroup.hpp
#pragma once
#include "ISystem.hpp"
#include "SystemGraph.hpp"

class ISystemGroup : public ISystem {
public:
    virtual ~ISystemGroup() = default;
    virtual SystemGraph& childGraph() = 0;
    virtual const SystemGraph& childGraph() const = 0;
};
