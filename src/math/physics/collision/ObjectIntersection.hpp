#pragma once
#include <algorithm>
#include "math/Vec3.hpp"
#include "math/Bounds3D.hpp"

class ObjectIntersection {
public:

    // ------------------------------------
    // AABB intersection test
    // ------------------------------------
    static bool intersects(const Bounds3D& a, const Bounds3D& b) {
        return (a.min.x < b.max.x && a.max.x > b.min.x) &&
            (a.min.y < b.max.y && a.max.y > b.min.y) &&
            (a.min.z < b.max.z && a.max.z > b.min.z);
    }

    // ------------------------------------
    // Overlapping volume
    // ------------------------------------
    static Bounds3D calculateIntersection(
        const Bounds3D& a,
        const Bounds3D& b
    ) {
        Bounds3D overlap{
            {
                std::max(a.min.x, b.min.x),
                std::max(a.min.y, b.min.y),
                std::max(a.min.z, b.min.z)
            },
            {
                std::min(a.max.x, b.max.x),
                std::min(a.max.y, b.max.y),
                std::min(a.max.z, b.max.z)
            }
        };

        return overlap.isEmpty() ? Bounds3D{} : overlap;
    }

    // ------------------------------------
    // Penetration / Minimum Translation Vector
    // ------------------------------------
    static Vec3 calculatePenetration(
        const Bounds3D& a,
        const Bounds3D& b
    ) {
        Bounds3D overlap = calculateIntersection(a, b);
        if (overlap.isEmpty()) {
            return Vec3{ 0.f, 0.f, 0.f };
        }

        Vec3 overlapSize = overlap.size();

        // Find smallest penetration axis
        if (overlapSize.x <= overlapSize.y &&
            overlapSize.x <= overlapSize.z) {

            float dir = (a.center().x < b.center().x) ? -1.f : 1.f;
            return Vec3{ overlapSize.x * dir, 0.f, 0.f };
        }

        if (overlapSize.y <= overlapSize.x &&
            overlapSize.y <= overlapSize.z) {

            float dir = (a.center().y < b.center().y) ? -1.f : 1.f;
            return Vec3{ 0.f, overlapSize.y * dir, 0.f };
        }

        float dir = (a.center().z < b.center().z) ? -1.f : 1.f;
        return Vec3{ 0.f, 0.f, overlapSize.z * dir };
    }
};