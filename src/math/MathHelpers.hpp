#pragma once

#include "Vec3.hpp"
#include "Vec2.hpp"
#include <cmath>
class MathHelpers {
public:
    struct ClosestPoints {
        float tBall;      // 0.0 to 1.0 along ball path
        float tRacket;    // 0.0 to 1.0 along racket path
        Vec3 posBall;     // Exact 3D point on ball path
        Vec3 posRacket;   // Exact 3D point on racket path
    };

    static Vec3 normalize(const Vec3& v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len < 0.00001f) return { 0,0,0 };
        return { v.x / len, v.y / len, v.z / len };
    }

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
    static Vec3 compMul(const Vec3& a, const Vec3& b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }

    static ClosestPoints findClosestPoints(Vec3 p1, Vec3 q1, Vec3 p2, Vec3 q2) {
        Vec3 d1 = q1 - p1; // Ball dir
        Vec3 d2 = q2 - p2; // Racket dir
        Vec3 r = p1 - p2;
        float a = d1.dot(d1);
        float e = d2.dot(d2);
        float f = d2.dot(r);
        float s, t;

        constexpr float EPSILON = 1e-6f;

        if (a <= EPSILON && e <= EPSILON) {
            s = t = 0.0f;
        }
        else if (a <= EPSILON) {
            s = 0.0f;
            t = std::clamp(f / e, 0.0f, 1.0f);
        }
        else {
            float c = d1.dot(r);
            if (e <= EPSILON) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else {
                float b = d1.dot(d2);
                float denom = a * e - b * b;
                if (denom != 0.0f) {
                    s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
                }
                else {
                    s = 0.0f; // Parallel lines
                }
                t = (b * s + f) / e;
                if (t < 0.0f) {
                    t = 0.0f;
                    s = std::clamp(-c / a, 0.0f, 1.0f);
                }
                else if (t > 1.0f) {
                    t = 1.0f;
                    s = std::clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }
        return { s, t, p1 + d1 * s, p2 + d2 * t };
    }
    static Vec3 MathHelpers::closestPointOnSegment(Vec3 a, Vec3 b, Vec3 p) {
        Vec3 ab = b - a;
        float lengthSq = ab.lengthSq();

        // Case 1: The segment is a single point (a == b)
        if (lengthSq < 1e-6f) return a;

        // Case 2: Project point p onto the line ab, 
        // finding the parameter 't' of the projection
        float t = (p - a).dot(ab) / lengthSq;

        // Case 3: Clamp t to the range [0, 1] to stay on the segment
        if (t < 0.0f) return a; // p is "behind" a
        if (t > 1.0f) return b; // p is "past" b

        // Return the projected point
        return a + ab * t;
    }
    static Vec3 MathHelpers::rotateByEuler(const Vec3& v, const Vec3& eulerRad) {
        Vec3 out = v;

        // --- Yaw (Y axis) ---
        if (eulerRad.y != 0.0f) {
            float c = std::cos(eulerRad.y);
            float s = std::sin(eulerRad.y);
            out = {
                out.x * c + out.z * s,
                out.y,
               -out.x * s + out.z * c
            };
        }

        // --- Pitch (X axis) ---
        if (eulerRad.x != 0.0f) {
            float c = std::cos(eulerRad.x);
            float s = std::sin(eulerRad.x);
            out = {
                out.x,
                out.y * c - out.z * s,
                out.y * s + out.z * c
            };
        }

        // --- Roll (Z axis) ---
        if (eulerRad.z != 0.0f) {
            float c = std::cos(eulerRad.z);
            float s = std::sin(eulerRad.z);
            out = {
                out.x * c - out.y * s,
                out.x * s + out.y * c,
                out.z
            };
        }

        return out;
    }
    static Vec3 rotationFromToEuler(const Vec3& from, const Vec3& to) {
        Vec3 f = from.normalized();
        Vec3 t = to.normalized();
        Vec3 axis = f.cross(t);
        float d = std::clamp(f.dot(t), -1.f, 1.f);
        float angle = std::acos(d);
        if (axis.lengthSq() < 1e-6f || angle < 1e-6f) return { 0,0,0 };
        axis = axis.normalized();
        return axis * angle;
    }

};