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

    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const { float len = length(); return len ? *this / len : Vec3(); }
    // Dot product of two vectors
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    // Returns the squared magnitude of the vector: x^2 + y^2 + z^2
    float lengthSq() const {
        return x * x + y * y + z * z;
    }

    Vec3 cross(const Vec3& other) {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }
    void set(const float f) {
        x = f;
        y = f;
        z = f;
    }
};
