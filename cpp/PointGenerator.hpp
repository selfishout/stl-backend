#pragma once

#include "Point.hpp"

#include <random>

namespace MF
{
    template <typename T>
    class PointGenerator
    {

    public:

        PointGenerator() = default;

        // halton points in [0, 1]^dim
        // "A Primer on RAdial Basis Functions with Applications to the Geosciences"
        std::vector<Point<T>> halton(size_t numpts, size_t ndims);

        // reject points randomly according to the edge length requirement, signedDist is the signed distance to boundary surface
        std::pair<std::vector<Point<T>>, std::vector<T>> reject(const std::vector<Point<T>>& points, const std::vector<T>& signedDist, const std::vector<T>& params);

        // same as Matlab meshgrid
        std::tuple<std::vector<std::vector<T>>, std::vector<std::vector<T>>> meshgrid(const std::vector<T>& x, const std::vector<T>& y);

        // generate evenly spaced numbers over a given range
        std::vector<T> arange(T b, T e, T stride = 1);

        // copy a range of a vector and scale, inclusive of ib and ie
        std::vector<T> copy_scale(const std::vector<T>& v, size_t ib, size_t ie, T s = 1);

        // add two matrices together
        std::vector<std::vector<T>> mat_add(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b);

        // concatenate all columns of a matrix into 1 column
        std::vector<T> toOneCol(const std::vector<std::vector<T>>& A);
    };

}