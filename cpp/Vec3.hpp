#pragma once

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <memory>
#include <array>
#include <numeric>
#include <iostream>

namespace MF
{
    template<typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3() : x(0), y(0), z(0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

        // Tolerance for floating point comparisons
        static constexpr T epsilon = 1e-12;

        bool operator==(const Vec3<T>& other) const
        {
            return std::abs(x - other.x) < epsilon &&
                std::abs(y - other.y) < epsilon &&
                std::abs(z - other.z) < epsilon;
        }

        Vec3 operator+(const Vec3& v) const
        {
            return Vec3(x + v.x, y + v.y, z + v.z);
        }

        Vec3 operator-(const Vec3& v) const
        {
            return Vec3(x - v.x, y - v.y, z - v.z);
        }

        Vec3 operator*(T s) const
        {
            return Vec3(x * s, y * s, z * s);
        }

        T dot(const Vec3& v) const
        {
            return x * v.x + y * v.y + z * v.z;
        }

        Vec3 cross(const Vec3& v) const
        {
            return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
        }

        T length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        T lengthSq() const
        {
            return x * x + y * y + z * z;
        }

        Vec3 normalize() const
        {
            T len = length();
            return len > T(1e-12) ? Vec3(x / len, y / len, z / len) : Vec3(0, 0, 0);
        }

        // convert [x, y, z] using the rotation matrix Rot and the shift vector shft
        // if Rot is diagonal, it is a scaling matrix
        void transform(const std::vector<std::vector<T>>& Rot, const std::vector<T>& shft)
        {
            std::vector<T> x0{ x, y, z };
            std::vector<T> res(3, 0);

            for (size_t ir = 0; ir < 3; ++ir)
            {
                for (size_t ic = 0; ic < 3; ++ic)
                {
                    res[ir] += Rot[ir][ic] * x0[ic];
                }
            }

            x = res[0] + shft[0];
            y = res[1] + shft[1];
            z = res[2] + shft[2];
        }


        // Component-wise min/max for bounding box operations

        Vec3 min(const Vec3& v) const
        {
            return Vec3(std::min(x, v.x), std::min(y, v.y), std::min(z, v.z));
        }

        Vec3 max(const Vec3& v) const
        {
            return Vec3(std::max(x, v.x), std::max(y, v.y), std::max(z, v.z));
        }
    };

    template<typename T>
    struct Vec3TypeHash
    {
        std::size_t operator()(const Vec3<T>& obj) const {
            // Combine hashes of individual members
            std::size_t h1 = std::hash<T>{}(obj.x);
            std::size_t h2 = std::hash<T>{}(obj.y);

            // Simple combination (can be more sophisticated)
            return h1 ^ (h2 << 1);
        }
    };
} 