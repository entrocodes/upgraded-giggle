#pragma once
#include <cstdint>
#include <string>

struct Entity {
    std::uint32_t id = 0;
    std::string name;

    bool operator==(const Entity& other) const noexcept {
        return id == other.id;
    }
};

// Hash function so Entity can be used as a key in unordered_map
namespace std {
    template <>
    struct hash<Entity> {
        std::size_t operator()(const Entity& e) const noexcept {
            return std::hash<std::uint32_t>()(e.id);
        }
    };
}

const Entity INVALID_ENTITY = { 0, "" };
