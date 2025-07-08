#pragma once

#include "Vec3.hpp"

namespace MF
{
    template<typename T>
    struct Ray
    {
        Vec3<T> origin;
        Vec3<T> direction;

        Ray() = default;
        Ray(const Vec3<T>& o, const Vec3<T>& d) : origin(o), direction(d.normalize()) {}

        Vec3<T> at(T t) const { return origin + direction * t; }
    };
} 