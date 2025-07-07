#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <functional>

namespace MF
{
    template<typename T>
    struct Vec3
    {
        T x, y, z;
        
        Vec3() : x(0), y(0), z(0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
        
        Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
        Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
        Vec3 operator*(T scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
        Vec3 operator/(T scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
        
        T dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }
        Vec3 cross(const Vec3& other) const { 
            return Vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); 
        }
        
        T length() const { return std::sqrt(x*x + y*y + z*z); }
        T lengthSquared() const { return x*x + y*y + z*z; }
        
        Vec3 normalized() const { 
            T len = length(); 
            return len > 0 ? *this / len : Vec3(); 
        }
    };
    
    template<typename T>
    using Point = Vec3<T>;
    
    template<typename T>
    struct Triangle
    {
        Vec3<T> v0, v1, v2;
        
        Triangle() = default;
        Triangle(const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2) : v0(v0), v1(v1), v2(v2) {}
        
        Vec3<T> normal() const {
            Vec3<T> edge1 = v1 - v0;
            Vec3<T> edge2 = v2 - v0;
            return edge1.cross(edge2).normalized();
        }
    };
    
    template<typename T>
    struct AABB
    {
        Vec3<T> min, max;
        
        AABB() = default;
        AABB(const Vec3<T>& min, const Vec3<T>& max) : min(min), max(max) {}
        
        bool contains(const Vec3<T>& point) const {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }
        
        T diagonal() const { return (max - min).length(); }
    };

    template<typename T>
    struct Ray
    {
        Vec3<T> origin;
        Vec3<T> direction;
        Ray(const Vec3<T>& o, const Vec3<T>& d) : origin(o), direction(d) {}
    };

    template<typename T>
    struct PointHash
    {
        std::size_t operator()(const Point<T>& p) const
        {
            std::size_t h1 = std::hash<T>{}(p.x);
            std::size_t h2 = std::hash<T>{}(p.y);
            std::size_t h3 = std::hash<T>{}(p.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
} 