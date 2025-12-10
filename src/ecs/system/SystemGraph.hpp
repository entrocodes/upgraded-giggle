#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>

#include "../game/utils/GameContext.hpp"
#include "SystemNode.hpp"
#include "SystemFactory.hpp"
#include "ISystem.hpp"
// --------------------------------------------
// Pause flags (readable call sites)
// --------------------------------------------
constexpr bool Pausable = true;
constexpr bool NotPausable = false;

class SystemGraph {
public:
    template<typename T, typename... Args>
    void add(SystemFactory& factory,
        int order,
        TickPhase phase,
        bool pausable = NotPausable,
        Args&&... args);

    SystemNode& addNode(SystemNode&& node);

    void run(GameContext* context, TickPhase phase);
    
    const std::vector<SystemNode>& getNodes() const { return m_nodes; }

private:
    void sortIfDirty();
    std::vector<SystemNode>  m_nodes;
    std::vector<SystemNode*> m_sortedNodes;
    bool m_dirty = true;
};

// --------------------------------------------
// Template implementation (MUST be in header)
// --------------------------------------------
template<typename T, typename... Args>
void SystemGraph::add(SystemFactory& factory,
    int order,
    TickPhase phase,
    bool pausable,
    Args&&... args)
{
    static_assert(std::is_base_of_v<ISystem, T>,
        "SystemGraph::add<T>: T must derive from ISystem");

    SystemNode node;
    node.system = factory.create<T>(std::forward<Args>(args)...);
    node.order = order;
    node.phase = phase;
    node.pausable = pausable;

    m_nodes.push_back(std::move(node));
    m_dirty = true;
}
