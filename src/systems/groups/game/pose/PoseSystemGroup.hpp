#pragma once

#include "game/utils/GameContext.hpp"


#include "ecs/system/ISystemGroup.hpp"

class PoseSystemGroup final : public ISystemGroup {
public:
    explicit PoseSystemGroup(SystemFactory& factory);

    SystemExec update(GameContext* context) override;

    SystemGraph& childGraph() override { return m_dummy; }
    const SystemGraph& childGraph() const override { return m_dummy; }

private:
    SystemFactory m_factory;

    // Per-stage subgraphs
    SystemGraph m_stagedGraph;

    // Final cleanup
    SystemGraph m_finalClearGraph;

    // Dummy graph to satisfy interface
    SystemGraph m_dummy;
};
