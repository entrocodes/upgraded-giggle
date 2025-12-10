#pragma once

#include "ISystem.hpp"
#include "SystemGraph.hpp"
#include "SystemFactory.hpp"

class SystemGroup final : public ISystem {
public:
    explicit SystemGroup(GameContext* context)
        : m_factory(context) {
    }

protected:
    // For derived groups to configure
    void addSystems() {}

    SystemGraph   m_graph;
    SystemFactory m_factory;

public:
    void update(GameContext* context, TickPhase phase) override {
        m_graph.run(context, phase);
    }
};
1