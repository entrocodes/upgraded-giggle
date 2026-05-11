#pragma once

#include "systems/game/racket/strokesystem/RacketStrokeStateSystem.hpp"
#include "systems/game/racket/strokesystem/RacketBackswingSystem.hpp"
#include "systems/game/racket/strokesystem/RacketIdleSystem.hpp"
#include "systems/game/racket/strokesystem/RacketPushSystem.hpp"
#include "systems/game/racket/strokesystem/RacketPushRecoverySystem.hpp"
#include "systems/game/racket/strokesystem/RacketPelvisSwingSystem.hpp"
#include "systems/game/racket/strokesystem/RacketSwingRecoverySystem.hpp"
#include "game/utils/GameContext.hpp"
#include "ecs/system/ISystemGroup.hpp"

class RacketStrokeSystemGroup final : public ISystemGroup {
public:
    explicit RacketStrokeSystemGroup(SystemFactory& factory)
        : m_factory(factory)
    {
        m_graph.add<RacketStrokeStateSystem>(m_factory, 0, TickPhase::Fixed);
        m_graph.add<RacketBackswingSystem>(m_factory, 25, TickPhase::Fixed); //will become wrist rotation
        m_graph.add<RacketIdleSystem>(m_factory, 50, TickPhase::Fixed);
        m_graph.add<RacketPushSystem>(m_factory, 150, TickPhase::Fixed);
        m_graph.add<RacketPushRecoverySystem>(m_factory, 150, TickPhase::Fixed);
        m_graph.add<RacketPelvisSwingSystem>(m_factory, 300, TickPhase::Fixed);
        m_graph.add<RacketSwingRecoverySystem>(m_factory, 300, TickPhase::Fixed);
    }

    SystemExec update(GameContext* context) override {
        m_graph.run(context, TickPhase::Fixed);
        return { SystemExecResult::Ran };
    }
    SystemGraph& childGraph() override { return m_graph; }
    const SystemGraph& childGraph() const override { return m_graph; }
private:
    SystemFactory m_factory;
    SystemGraph   m_graph;
};
