#include "Octree.hpp"
#include "Point.hpp"

using namespace MF;

// Explicit template instantiations
template struct Vec3<float>;
template struct Vec3<double>;
template struct Vec3<long double>;

template struct Point<float>;
template struct Point<double>;
template struct Point<long double>;

template struct AABB<float>;
template struct AABB<double>;
template struct AABB<long double>;

template struct Triangle<float>;
template struct Triangle<double>;
template struct Triangle<long double>;

template class Octree<Triangle<float>, float>;
template class Octree<Triangle<double>, double>;
template class Octree<Triangle<long double>, long double>;

template class Octree<Point<float>, float>;
template class Octree<Point<double>, double>;
template class Octree<Point<long double>, long double>;

// Type aliases for convenience with different numeric types
template<typename CoordType = float>
using PointOctree = Octree<Point<CoordType>, CoordType>;

template<typename CoordType = float>
using TriangleOctree = Octree<Triangle<CoordType>, CoordType>; 