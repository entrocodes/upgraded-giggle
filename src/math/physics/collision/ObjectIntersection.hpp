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
    static bool sweepSegmentAABB(
        const Vec3& p0,
        const Vec3& p1,
        const Bounds3D& box,
        float& outT)
    {
        Vec3 d = p1 - p0;

        float tMin = 0.0f;
        float tMax = 1.0f;

        auto testAxis = [&](float p, float dp, float minB, float maxB) -> bool
            {
                if (std::abs(dp) < 1e-6f)
                    return (p >= minB && p <= maxB);

                float invD = 1.0f / dp;
                float t0 = (minB - p) * invD;
                float t1 = (maxB - p) * invD;
                if (t0 > t1) std::swap(t0, t1);

                tMin = std::max(tMin, t0);
                tMax = std::min(tMax, t1);
                return tMin <= tMax;
            };

        if (!testAxis(p0.x, d.x, box.min.x, box.max.x)) return false;
        if (!testAxis(p0.y, d.y, box.min.y, box.max.y)) return false;
        if (!testAxis(p0.z, d.z, box.min.z, box.max.z)) return false;

        outT = tMin;
        return true;
    }
};