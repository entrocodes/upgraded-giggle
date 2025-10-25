#pragma once
#include <random>
#include "../math/Vec2.hpp"  // assuming your Vec2 struct is here

namespace Math {

    class Random {
    public:
        // Generates a random float between min and max, snapped to the given increment.
        static float range(float min, float max, float increment = 0.0f) {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_real_distribution<float> dist(min, max);

            float value = dist(rng);

            if (increment > 0.0f) {
                // Snap to nearest increment
                float snapped = std::round((value - min) / increment) * increment + min;
                // Clamp to ensure it stays within range
                if (snapped > max) snapped = max;
                if (snapped < min) snapped = min;
                return snapped;
            }

            return value;
        }

        // Generates a random Vec2 where each component is between its own min/max, with its own increment
        static Vec2 rangeVec2(
            Vec2 min, Vec2 max,
            Vec2 increment = Vec2{ 0.0f, 0.0f }
        ) {
            return Vec2{
                range(min.x, max.x, increment.x),
                range(min.y, max.y, increment.y)
            };
        }
    };

} // namespace Math
