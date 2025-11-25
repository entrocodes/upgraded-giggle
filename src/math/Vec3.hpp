#pragma once
#include <cmath>

struct Vec3 {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vec3 operator/(float s) const { return { x / s, y / s, z / s }; }
    Vec3 operator+(float s) const { return { x + s, y + s, z + s }; }
    Vec3 operator-(float s) const { return { x - s, y - s, z - s }; }

    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    Vec3& operator*=(const float s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(const float s) { x /= s; y /= s; z /= s; return *this; }
    Vec3& operator+=(const float s) { x += s; y += s; z += s; return *this; }
    Vec3& operator-=(const float s) { x -= s; y -= s; z -= s; return *this; }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const { float len = length(); return len ? *this / len : Vec3(); }
    // Dot product of two vectors
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    void set(const float f) {
        x = f;
        y = f;
        z = f;
    }
};
