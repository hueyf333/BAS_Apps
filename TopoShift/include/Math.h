#pragma once

#include <DirectXMath.h>
#include <cmath>
#include <algorithm>

using namespace DirectX;

namespace TopoShift {

// Basic 3D vector utilities
struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit Vec3(const XMFLOAT3& v) : x(v.x), y(v.y), z(v.z) {}

    XMFLOAT3 toXMFLOAT3() const { return XMFLOAT3(x, y, z); }
    XMVECTOR toXMVECTOR() const { return XMVectorSet(x, y, z, 0.0f); }

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }
    
    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }

    float dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }
    Vec3 cross(const Vec3& other) const {
        return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }
    Vec3 normalized() const {
        float len = length();
        return len > 1e-8f ? (*this / len) : Vec3(0, 0, 0);
    }
};

// Transform (rotation + translation + uniform scale)
struct Transform {
    XMFLOAT3 translation;
    XMFLOAT4 rotation; // Quaternion
    float scale;

    Transform() : translation(0, 0, 0), rotation(0, 0, 0, 1), scale(1.0f) {}

    XMMATRIX toMatrix() const {
        XMVECTOR t = XMLoadFloat3(&translation);
        XMVECTOR r = XMLoadFloat4(&rotation);
        XMVECTOR s = XMVectorReplicate(scale);
        return XMMatrixScalingFromVector(s) * XMMatrixRotationQuaternion(r) * XMMatrixTranslationFromVector(t);
    }

    static Transform fromMatrix(const XMMATRIX& mat) {
        Transform result;
        XMVECTOR s, r, t;
        XMMatrixDecompose(&s, &r, &t, mat);
        XMStoreFloat3(&result.translation, t);
        XMStoreFloat4(&result.rotation, r);
        XMStoreFloat(&result.scale, s);
        return result;
    }

    Vec3 transformPoint(const Vec3& p) const {
        XMVECTOR vp = XMVectorSet(p.x, p.y, p.z, 1.0f);
        XMVECTOR result = XMVector3Transform(vp, toMatrix());
        XMFLOAT3 fresult;
        XMStoreFloat3(&fresult, result);
        return Vec3(fresult);
    }
};

// Ray for picking
struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() = default;
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d.normalized()) {}

    Vec3 at(float t) const { return origin + direction * t; }
};

// Axis-aligned bounding box
struct AABB {
    Vec3 min;
    Vec3 max;

    AABB() : min(FLT_MAX, FLT_MAX, FLT_MAX), max(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}
    AABB(const Vec3& min, const Vec3& max) : min(min), max(max) {}

    void expand(const Vec3& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    void expand(const AABB& other) {
        expand(other.min);
        expand(other.max);
    }

    Vec3 center() const { return (min + max) * 0.5f; }
    Vec3 extents() const { return (max - min) * 0.5f; }

    bool intersects(const Ray& ray, float& tMin, float& tMax) const {
        // Slab method
        tMin = 0.0f;
        tMax = FLT_MAX;

        for (int i = 0; i < 3; ++i) {
            float origin = i == 0 ? ray.origin.x : (i == 1 ? ray.origin.y : ray.origin.z);
            float dir = i == 0 ? ray.direction.x : (i == 1 ? ray.direction.y : ray.direction.z);
            float bmin = i == 0 ? min.x : (i == 1 ? min.y : min.z);
            float bmax = i == 0 ? max.x : (i == 1 ? max.y : max.z);

            if (std::abs(dir) < 1e-8f) {
                if (origin < bmin || origin > bmax) return false;
            } else {
                float invD = 1.0f / dir;
                float t0 = (bmin - origin) * invD;
                float t1 = (bmax - origin) * invD;
                if (t0 > t1) std::swap(t0, t1);
                tMin = std::max(tMin, t0);
                tMax = std::min(tMax, t1);
                if (tMin > tMax) return false;
            }
        }
        return true;
    }
};

// Triangle-ray intersection (Möller–Trumbore algorithm)
inline bool intersectTriangle(const Ray& ray, const Vec3& v0, const Vec3& v1, const Vec3& v2,
                               float& t, float& u, float& v) {
    const float EPSILON = 1e-8f;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON) return false; // Ray parallel to triangle

    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) return false;

    Vec3 q = s.cross(edge1);
    v = f * ray.direction.dot(q);
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * edge2.dot(q);
    return t > EPSILON;
}

// Compute barycentric coordinates of point p in triangle (v0, v1, v2)
inline Vec3 computeBarycentric(const Vec3& p, const Vec3& v0, const Vec3& v1, const Vec3& v2) {
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 v0p = p - v0;

    float d00 = v0v1.dot(v0v1);
    float d01 = v0v1.dot(v0v2);
    float d11 = v0v2.dot(v0v2);
    float d20 = v0p.dot(v0v1);
    float d21 = v0p.dot(v0v2);

    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-8f) return Vec3(1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f);

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vec3(u, v, w);
}

// Interpolate using barycentric coordinates
inline Vec3 barycentricInterpolate(const Vec3& bary, const Vec3& v0, const Vec3& v1, const Vec3& v2) {
    return v0 * bary.x + v1 * bary.y + v2 * bary.z;
}

} // namespace TopoShift
