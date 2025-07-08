#pragma once

#include "Vec3.hpp"
#include "Ray.hpp"

#include <algorithm>
#include <limits>

namespace MF
{
    template<typename T>
    struct AABB
    {
        Vec3<T> min, max;

        AABB() : min(0, 0, 0), max(0, 0, 0) {}

        AABB(const Vec3<T>& min, const Vec3<T>& max) : min(min), max(max) {}

        Vec3<T> center() const { return (min + max) * T(0.5); }

        Vec3<T> size() const { return max - min; }

        bool contains(const Vec3<T>& point) const
        {
            return point.x >= min.x && point.x <= max.x &&
                point.y >= min.y && point.y <= max.y &&
                point.z >= min.z && point.z <= max.z;
        }

        bool intersects(const AABB& other) const
        {
            return min.x <= other.max.x && max.x >= other.min.x &&
                min.y <= other.max.y && max.y >= other.min.y &&
                min.z <= other.max.z && max.z >= other.min.z;
        }

        void expand(const Vec3<T>& point)
        {
            min = min.min(point);
            max = max.max(point);
        }

        void expand(const AABB& box)
        {
            min = min.min(box.min);
            max = max.max(box.max);
        }

        // Ray-AABB intersection using slab method
        bool rayIntersect(const Ray<T>& ray, T& tMin, T& tMax) const
        {
            Vec3<T> invDir = Vec3<T>(T(1.0) / ray.direction.x, T(1.0) / ray.direction.y, T(1.0) / ray.direction.z);

            T t1 = (min.x - ray.origin.x) * invDir.x;
            T t2 = (max.x - ray.origin.x) * invDir.x;
            if (t1 > t2) std::swap(t1, t2);

            T tymin = (min.y - ray.origin.y) * invDir.y;
            T tymax = (max.y - ray.origin.y) * invDir.y;
            if (tymin > tymax) std::swap(tymin, tymax);

            if (t1 > tymax || tymin > t2) return false;

            t1 = std::max(t1, tymin);
            t2 = std::min(t2, tymax);

            T tzmin = (min.z - ray.origin.z) * invDir.z;
            T tzmax = (max.z - ray.origin.z) * invDir.z;
            if (tzmin > tzmax) std::swap(tzmin, tzmax);

            if (t1 > tzmax || tzmin > t2) return false;

            tMin = std::max(t1, tzmin);
            tMax = std::min(t2, tzmax);

            return tMax >= 0;
        }
    };
} 