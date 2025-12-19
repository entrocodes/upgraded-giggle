#pragma once
#include "math/Vec3.hpp"
#include "math/Bounds3D.hpp"
class BallObjectIntersection {
public:

    static Bounds3D calculateIntersection(const Bounds3D a, const Bounds3D b) {
        if (!intersects(a, b)) {
            // return empty bounds
            return Bounds3D({ 0,0,0 }, { 0,0,0 });
        }
        Vec3 minOverlap = {
            std::max(a.min.x, b.min.x),
            std::max(a.min.y, b.min.y),
            std::max(a.min.z, b.min.z)
        };

        Vec3 maxOverlap = {
            std::min(a.max.x, b.max.x),
            std::min(a.max.y, b.max.y),
            std::min(a.max.z, b.max.z)
        };

        return Bounds3D(minOverlap, maxOverlap);
    };
    static bool intersects(const Bounds3D a, const Bounds3D b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
            (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
            (a.min.z <= b.max.z && a.max.z >= b.min.z);
    };

};