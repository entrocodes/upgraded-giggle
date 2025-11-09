#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>
#include <tuple>
#include <string>
#include "Entity.hpp"
#include "Component.hpp"

class Registry {
public:
    // --- Entity Creation ---
    Entity createEntity() {
        Entity e;
        e.id = ++m_nextEntityId;
        m_entities.push_back(e);
        return e;
    }

    Entity createEntity(const std::string& name) {
        Entity e;
        e.id = ++m_nextEntityId;
        e.name = name;
        m_entities.push_back(e);
        m_nameLookup[name] = &m_entities.back();
        return e;
    }

    // --- Entity Lookup ---
    Entity* getEntity(const std::string& name) {
        auto it = m_nameLookup.find(name);
        return (it != m_nameLookup.end()) ? it->second : nullptr;
    }

    size_t getEntityCount() const {
        return m_entities.size();
    }

    // --- Component Management ---
    template <typename T, typename... Args>
    T& addComponent(const Entity& e, Args&&... args) {
        auto& storage = m_components[typeid(T)];
        if (!storage) {
            storage = std::make_unique<std::unordered_map<Entity, std::unique_ptr<Component>>>();
        }

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *component;
        (*storage)[e] = std::move(component);
        return ref;
    }

    template <typename T>
    T* getComponent(const Entity& e) const {
        auto it = m_components.find(typeid(T));
        if (it == m_components.end()) return nullptr;
        auto& map = *it->second;
        auto found = map.find(e);
        if (found == map.end()) return nullptr;
        return static_cast<T*>(found->second.get());
    }

    template <typename T>
    bool hasComponent(const Entity& e) const {
        auto it = m_components.find(typeid(T));
        if (it == m_components.end()) return false;
        auto& map = *it->second;
        return map.find(e) != map.end();
    }

    template <typename... Components>
    std::vector<Entity> getEntitiesWith() const {
        std::vector<Entity> result;
        for (const auto& e : m_entities) {
            if (hasAllComponents<Components...>(e)) {
                result.push_back(e);
            }
        }
        return result;
    }

    template <typename... Components>
    auto getComponents(const Entity& e) {
        return std::make_tuple(getComponent<Components>(e)...);
    }

private:
    std::uint32_t m_nextEntityId = 0;
    std::vector<Entity> m_entities;
    std::unordered_map<std::string, Entity*> m_nameLookup;

    std::unordered_map<
        std::type_index,
        std::unique_ptr<std::unordered_map<Entity, std::unique_ptr<Component>>>>
        m_components;

    template <typename First, typename... Rest>
    bool hasAllComponents(const Entity& e) const {
        if (!hasComponent<First>(e)) return false;
        if constexpr (sizeof...(Rest) == 0)
            return true;
        else
            return hasAllComponents<Rest...>(e);
    }
};
