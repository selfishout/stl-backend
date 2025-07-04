#include "PointGenerator.hpp"

using namespace MF;

//

template <typename T>
std::vector<Point<T>> PointGenerator<T>::halton(size_t numpts, size_t ndims)
{
    std::vector<int> p{ 2, 3, 5, 7, 11, 13 };

    std::vector<Point<T>> H(numpts, Point<T>());

    for (size_t k = 0; k < ndims; ++k)
    {
        int N = p[k];
        // std::cout << "N = " << N; exit(1);
        std::vector<T> v1{ 0 };
        std::vector<T> v2 = arange(0, N - 1);
        // for (auto& tmp : v2) std::cout << tmp << ", "; exit(1);
        size_t lv1 = 1;
        while (lv1 <= numpts)
        {
            size_t ie = (size_t)std::max(2.0, std::min((double)(N), std::ceil(((double)(numpts + 1) / lv1))));
            v2 = copy_scale(v2, 0, ie - 1, 1.0 / N);
            // for (auto& tmp : v2a) std::cout << tmp << ", "; exit(1);

            auto [x1, x2] = meshgrid(v2, v1);
            // for (auto& tmp : x2) { for (auto& tmp1 : tmp) std::cout << tmp1 << ", ";  std::cout << "\n"; } exit(1);

            auto xx = mat_add(x1, x2);

            v1 = toOneCol(xx);
            // for (auto& tmp : v1) std::cout << tmp << " "; exit(1);
            lv1 = v1.size();
            // std::cout << "lv1 = " << lv1 << "\n"; exit(1);
        }
        for (size_t ii = 1; ii <= numpts; ++ii)
        {
            H[ii - 1][k] = v1[ii];
        }
    }

    return H;
}

//

template <typename T>
std::pair<std::vector<Point<T>>, std::vector<T>> PointGenerator<T>::reject(const std::vector<Point<T>>& points, const std::vector<T>& signedDist, const std::vector<T>& params)
{
    T hmin = params[0];
    T hmax = params[1];
    T scale = params[2];

    std::vector<T> r0;
    for (auto& d : signedDist)
    {
        r0.push_back(std::abs(d) / scale * (hmax - hmin) + hmin);
    }

    T r0_min = *std::min_element(r0.begin(), r0.end());

    size_t dim = 3;

    T r0_min_p = std::pow(r0_min, dim);

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    std::vector<Point<T>> h;
    std::vector<T> D;

    for (size_t i = 0; i < r0.size(); ++i)
    {
        double number = distribution(generator);
        if (number < r0_min_p / std::pow(r0[i], dim))
        {
            h.push_back(points[i]);
            D.push_back(signedDist[i]);
        }
    }

    return { h, D };
}


//

template <typename T>
std::tuple<std::vector<std::vector<T>>, std::vector<std::vector<T>>> PointGenerator<T>::meshgrid(const std::vector<T>& x, const std::vector<T>& y)
{
    std::vector<std::vector<T>> X(y.size(), std::vector<T>(x.size()));
    std::vector<std::vector<T>> Y(y.size(), std::vector<T>(x.size()));

    for (size_t iy = 0; iy < y.size(); ++iy)
    {
        for (size_t ix = 0; ix < x.size(); ++ix)
        {
            X[iy][ix] = x[ix];
            Y[iy][ix] = y[iy];
        }
    }
    return { X, Y };
}

//

template <typename T>
std::vector<std::vector<T>> PointGenerator<T>::mat_add(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b)
{
    std::vector<std::vector<T>> res(a.size(), std::vector<T>(a[0].size()));
    for (size_t ir = 0; ir < a.size(); ++ir)
    {
        for (size_t ic = 0; ic < a[0].size(); ++ic)
        {
            res[ir][ic] = a[ir][ic] + b[ir][ic];
        }
    }
    return res;
}

//

template <typename T>
std::vector<T> PointGenerator<T>::copy_scale(const std::vector<T>& v, size_t ib, size_t ie, T s)
{
    std::vector<T> res;
    for (size_t ii = ib; ii <= ie; ++ii)
    {
        res.push_back(v[ii] * s);
    }
    return res;
}

//

template <typename T>
std::vector<T> PointGenerator<T>::arange(T b, T e, T stride)
{
    std::vector<T> res;
    T v = b;
    while (v <= e)
    {
        res.push_back(v);
        v += stride;
    }
    return res;
}

//

template <typename T>
std::vector<T> PointGenerator<T>::toOneCol(const std::vector<std::vector<T>>& A)
{
    std::vector<T> res(A.size() * A[0].size());
    size_t ii = 0;
    for (size_t ic = 0; ic < A[0].size(); ++ic)
    {
        for (size_t ir = 0; ir < A.size(); ++ir)
        {
            res[ii++] = A[ir][ic];
        }
    }
    return res;
}


//

template class PointGenerator<float>;
template class PointGenerator<double>;
template class PointGenerator<long double>;

//

#ifdef TEST_PG

int main()
{
    PointGenerator<float> pgf;

    size_t numpts = 5000;
    size_t ndim = 3;
    auto H = pgf.halton(numpts, ndim);
    // std::cout << H.size() << "\n";

    std::vector<std::vector<float>> Rot{ {18, 0, 0}, {0, 11, 0}, {0, 0, 9} };
    std::vector<float> shft{ -8.5, -5.5, 0 };
    for (auto& p : H)
    {
        p.position.transform(Rot, shft);
        std::cout << p.to_string() << "\n";
    }
}


#endif