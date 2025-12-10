#include "SystemGraph.hpp"

// Run systems in a given phase
void SystemGraph::run(GameContext* context, TickPhase phase)
{
    sortIfDirty();

    for (auto* node : m_sortedNodes) {
        if (!node->enabled) continue;
        if (node->phase != phase) continue;
        if (context->metaInputState.paused && node->pausable) continue;

        // ✅ Run ONCE
        SystemExec exec = node->system->update(context);

        // ✅ Record stats
        auto& dbg = node->debug;
        dbg.lastFrameRan = context->frameStats.frameIndex;
        dbg.runCount++;

        dbg.exitedEarlyLastRun = (exec.result == SystemExecResult::EarlyExit);

        if (dbg.exitedEarlyLastRun) {
            dbg.earlyExitCount++;
            dbg.lastEarlyExitReason =
                exec.earlyExitReason ? exec.earlyExitReason : "";
        }
        else {
            dbg.lastEarlyExitReason.clear();
        }
    }
}



// Add pre-built node
SystemNode& SystemGraph::addNode(SystemNode&& node) {
    m_nodes.push_back(std::move(node));
    m_dirty = true;
    return m_nodes.back();
}

// Sort system nodes by order
void SystemGraph::sortIfDirty() {
    if (!m_dirty) return;

    m_sortedNodes.clear();
    m_sortedNodes.reserve(m_nodes.size());

    for (auto& node : m_nodes)
        m_sortedNodes.push_back(&node);

    std::sort(
        m_sortedNodes.begin(),
        m_sortedNodes.end(),
        [](const SystemNode* a, const SystemNode* b) {
            return a->order < b->order;
        }
    );

    m_dirty = false;
}
