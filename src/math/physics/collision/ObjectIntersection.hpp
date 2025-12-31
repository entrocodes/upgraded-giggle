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
    // Checks if two bounds overlap specifically on the Z axis
    static bool overlapsZ(const Bounds3D& a, const Bounds3D& b) {
        return a.min.z < b.max.z && a.max.z > b.min.z;
    }

    // Generic version for any axis if needed
    static bool overlapsX(const Bounds3D& a, const Bounds3D& b) {
        return a.min.x < b.max.x && a.max.x > b.min.x;
    }
};