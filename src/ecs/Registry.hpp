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
        m_nameLookup[name] = e;  // store a copy, not a pointer
        return e;
    }

    Entity* getEntity(const std::string& name) {
        auto it = m_nameLookup.find(name);
        if (it == m_nameLookup.end()) return nullptr;

        for (auto& ent : m_entities) {
            if (ent.id == it->second.id)
                return &ent;
        }
        return nullptr;
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
    void deleteEntity(const Entity& e) {
        // Remove from all component storages
        for (auto& [type, storagePtr] : m_components) {
            auto& compMap = *storagePtr;
            compMap.erase(e);
        }

        // 2️⃣ Remove from the entity list
        m_entities.erase(
            std::remove_if(
                m_entities.begin(),
                m_entities.end(),
                [&](const Entity& ent) { return ent.id == e.id; }),
            m_entities.end()
        );

        // 3️ Remove from name lookup if it exists
        for (auto it = m_nameLookup.begin(); it != m_nameLookup.end(); ) {
            if (it->second.id == e.id)
                it = m_nameLookup.erase(it);
            else
                ++it;
        }
    }
    void removeAllEntities() {
        m_entities.clear();
        m_nameLookup.clear();
        m_components.clear();
        m_nextEntityId = 0;
    }
    template <typename T>
    void removeComponent(const Entity& e) {
        auto it = m_components.find(typeid(T));
        if (it == m_components.end()) return;

        auto& map = *it->second;
        map.erase(e);
    }
    template <typename T, typename... Args>
    T& getOrAddComponent(const Entity& e, Args&&... args) {
        if (auto* c = getComponent<T>(e))
            return *c;
        return addComponent<T>(e, std::forward<Args>(args)...);
    }
private:
    std::uint32_t m_nextEntityId = 0;
    std::vector<Entity> m_entities;
    std::unordered_map<std::string, Entity> m_nameLookup;


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
