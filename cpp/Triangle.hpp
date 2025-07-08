#pragma once

#include "AABB.hpp"
#include "Point.hpp"

#include <iostream>

namespace MF
{
    template<typename T>
    struct Triangle
    {
        Vec3<T> v0, v1, v2;
        int id;
        int v_idx[3]; // Indices into the global points vector

        Vec3<T> normal;

        // AABB<T> bbox;

        // Basic constructors

        Triangle() {}

        Triangle(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c) : v0(a), v1(b), v2(c)
        {
            Vec3<T> edge1 = v1 - v0;
            Vec3<T> edge2 = v2 - v0;
            normal = edge1.cross(edge2).normalize();

            // Compute bounding box
            // bbox.expand(v0);
            // bbox.expand(v1);
            // bbox.expand(v2);
        }

        Triangle(const Vec3<T>& a, const Vec3<T>& b, const Vec3<T>& c, int id_) : v0(a), v1(b), v2(c), id(id_)
        {
            Vec3<T> edge1 = v1 - v0;
            Vec3<T> edge2 = v2 - v0;
            normal = edge1.cross(edge2).normalize();
        }

        Triangle(int i1, int i2, int i3, int _id = -1) : id(_id)
        {
            v_idx[0] = i1; v_idx[1] = i2; v_idx[2] = i3;
        }

        // required by Octree

        AABB<T> getBounds() const
        {
            Vec3<T> min_pt(std::min({ v0.x, v1.x, v2.x }),
                std::min({ v0.y, v1.y, v2.y }),
                std::min({ v0.z, v1.z, v2.z }));
            Vec3<T> max_pt(std::max({ v0.x, v1.x, v2.x }),
                std::max({ v0.y, v1.y, v2.y }),
                std::max({ v0.z, v1.z, v2.z }));
            return AABB<T>(min_pt, max_pt);
        }

        // required by Octree

        Vec3<T> center() const
        {
            return (v0 + v1 + v2) * T(1.0 / 3.0);
        }

        // IO

        void print() const
        {
            std::cout << "Triangle " << id << ":\n----------\n";
            std::cout << v0.x << ", " << v0.y << ", " << v0.z << "\n";
            std::cout << v1.x << ", " << v1.y << ", " << v1.z << "\n";
            std::cout << v2.x << ", " << v2.y << ", " << v2.z << "\n";
        }

        void print(const std::vector<Point<T>>& global_points) const
        {
            const Point<T>& p1 = global_points[v_idx[0]];
            const Point<T>& p2 = global_points[v_idx[1]];
            const Point<T>& p3 = global_points[v_idx[2]];

            std::cout << "Triangle " << id << ":\n----------\n";

            std::cout << p1.position.x << " " << p1.position.y << " " << p1.position.z << "\n";
            std::cout << p2.position.x << " " << p2.position.y << " " << p2.position.z << "\n";
            std::cout << p3.position.x << " " << p3.position.y << " " << p3.position.z << "\n";
        }

        void printCenter() const
        {
            Vec3<T> c = center();
            std::cout << c.x << ", " << c.y << ", " << c.z << "\n";
        }

        std::string to_string() const
        {
            return std::to_string(v0.x) + ", " + std::to_string(v0.y) + ", " + std::to_string(v0.z) + "\n"
                + std::to_string(v1.x) + ", " + std::to_string(v1.y) + ", " + std::to_string(v1.z) + "\n"
                + std::to_string(v2.x) + ", " + std::to_string(v2.y) + ", " + std::to_string(v2.z) + "\n";
        }
    };

} 