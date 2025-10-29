#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>
#include "Entity.hpp"
#include "Component.hpp"

class Registry {
public:
    // Create a new entity
    Entity createEntity() {
        Entity e = ++m_nextEntity;
        m_entities.push_back(e);
        return e;
    }

    // Add a component of type T to an entity
    template<typename T, typename... Args>
    T& addComponent(Entity e, Args&&... args) {
        auto& storage = m_components[typeid(T)];
        if (!storage) {
            storage = std::make_unique<std::unordered_map<Entity, std::unique_ptr<Component>>>();
        }

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        (*storage)[e] = std::move(component);
        return ref;
    }

    // Get a pointer to a component of type T from an entity
    template<typename T>
    T* getComponent(Entity e) const {
        auto it = m_components.find(typeid(T));
        if (it == m_components.end()) return nullptr;
        auto& map = *it->second;
        auto found = map.find(e);
        if (found == map.end()) return nullptr;
        return static_cast<T*>(found->second.get());
    }

    // Check if an entity has a component of type T
    template<typename T>
    bool hasComponent(Entity e) const {
        auto it = m_components.find(typeid(T));
        if (it == m_components.end()) return false;
        auto& map = *it->second;
        return map.find(e) != map.end();
    }

    // Get all entities that have all of the specified components
    template<typename... Components>
    std::vector<Entity> getEntitiesWith() const {
        std::vector<Entity> result;
        for (auto e : m_entities) {
            if (hasAllComponents<Components...>(e)) {
                result.push_back(e);
            }
        }
        return result;
    }
    size_t getEntityCount() const {
        return m_entities.size();
    }
private:
    Entity m_nextEntity = 0;
    std::vector<Entity> m_entities;

    // Map component type -> map of entity -> component instance
    std::unordered_map<std::type_index, std::unique_ptr<std::unordered_map<Entity, std::unique_ptr<Component>>>> m_components;

    // Helper function to recursively check component packs (works on MSVC)
    template<typename First, typename... Rest>
    bool hasAllComponents(Entity e) const {
        if (!hasComponent<First>(e)) return false;
        if constexpr (sizeof...(Rest) == 0)
            return true;
        else
            return hasAllComponents<Rest...>(e);
    }
};
