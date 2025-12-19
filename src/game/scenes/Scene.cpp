#include "Scene.hpp"
#include "ecs/system/SystemGraph.hpp"

SystemGraph& Scene::systems() {
    return *m_systems;
}

const SystemGraph& Scene::systems() const {
    return *m_systems;
}
