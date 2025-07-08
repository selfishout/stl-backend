#pragma once

#include "AABB.hpp"

#include <iostream>

namespace MF
{
    // Point structure template
    template<typename T>
    struct Point
    {
        Vec3<T> position;
        int id;

        Point() {}
        Point(const Vec3<T>& pos) : position(pos) {}
        Point(T x, T y, T z, int id_ = -1) : position(x, y, z), id{ id_ } {}

        AABB<T> getBounds() const { return AABB<T>(position, position); }
        Vec3<T> center() const { return position; }

        bool operator==(const Point<T>& other) const
        {
            return position == other.position;
        }

        T& operator[](size_t idx) { return (idx == 0) ? position.x : ((idx == 1) ? position.y : position.z); }

        const T& operator[](size_t idx) const { return (idx == 0) ? position.x : ((idx == 1) ? position.y : position.z); }

        // IO

        void print() const
        {
            std::cout << id << ": " << position.x << ", " << position.y << ", " << position.z << "\n";
        }

        std::string to_string() const
        {
            return std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z);
        }
    };

    template<typename T>
    struct PointHash
    {
        std::size_t operator()(const Point<T>& obj) const
        {
            // Combine hashes of individual members
            std::size_t h1 = std::hash<T>{}(obj.position.x);
            std::size_t h2 = std::hash<T>{}(obj.position.y);
            // std::size_t h1 = std::hash<int>{}(obj.id);
            // Simple combination (can be more sophisticated)
            return h1 ^ (h2 << 1);
            // return h1;
        }
    };
} 