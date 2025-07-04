#include "SignedDistanceCalculator.hpp"

using namespace MF;

//

// Function to find the closest point on a line segment to a given point
template <typename T>
Vec3<T> SignedDistanceCalculator<T>::closestPointOnSegment(const Vec3<T>& p, const Vec3<T>& a, const Vec3<T>& b) {
    Vec3<T> ab = b - a;
    T t = (p - a).dot(ab) / ab.lengthSq();
    t = std::max(static_cast<T>(0.0), std::min(static_cast<T>(1.0), t));
    return a + ab * t;
}


// Calculate closest point on triangle to query point
template <typename T>
Vec3<T> SignedDistanceCalculator<T>::closestPointOnTriangle(const Triangle<T>& tri, const Vec3<T>& p) const
{
    const Vec3<T>& a = tri.v0;
    const Vec3<T>& b = tri.v1;
    const Vec3<T>& c = tri.v2;

    Vec3<T> ab = b - a;
    Vec3<T> ac = c - a;
    Vec3<T> bc = c - b;

    // Compute the vector from A to P
    Vec3<T> ap = p - a;
    T d1 = ab.dot(ap);
    T d2 = ac.dot(ap);

    if (d1 <= 0 && d2 <= 0) {
        // The closest point is vertex A
        return a;
    }

    // Compute the vector from B to P
    Vec3<T> bp = p - b;
    T d3 = ab.dot(bp);
    T d4 = ac.dot(bp);

    if (d3 >= 0 && d4 <= d3) {
        // The closest point is vertex B
        return b;
    }

    // Check if P is in the edge region of AB
    T vc = d1 * d4 - d3 * d2;
    if (vc <= 0 && d1 >= 0 && d3 <= 0) {
        T v = d1 / (d1 - d3);
        return a + ab * v;
    }

    // Compute the vector from C to P
    Vec3<T> cp = p - c;
    T d5 = ab.dot(cp);
    T d6 = ac.dot(cp);

    if (d6 >= 0 && d5 <= d6) {
        // The closest point is vertex C
        return c;
    }

    // Check if P is in the edge region of AC
    T vb = d5 * d2 - d1 * d6;
    if (vb <= 0 && d2 >= 0 && d6 <= 0) {
        T w = d2 / (d2 - d6);
        return a + ac * w;
    }

    // Check if P is in the edge region of BC
    T va = d3 * d6 - d5 * d4;
    if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0) {
        T w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + bc * w;
    }

    // P is inside the triangle's face region. Project P onto the triangle's plane.
    T denom = static_cast<T>(1.0) / (va + vb + vc);
    T v = vb * denom;
    T w = vc * denom;
    return a + ab * v + ac * w;
}

//

// Calculate unsigned distance to surface using octree acceleration
template <typename T>
T SignedDistanceCalculator<T>::unsignedDistanceToSurface(const Vec3<T>& point, Vec3<T>& closestPoint) const
{
    // if (!octree)
    // {
    //     // Fallback to brute force if no octree
    //     T minDist = std::numeric_limits<T>::max();
    //     for (const auto& tri : triangles)
    //     {
    //         Vec3<T> closest = closestPointOnTriangle(tri, point);
    //         T dist = (point - closest).length();
    //         if (dist < minDist)
    //         {
    //             minDist = dist;
    //             closestPoint = closest;
    //         }
    //     }
    //     return minDist;
    // }

    T minDist = std::numeric_limits<T>::max();
    int k = 10;
    auto nearestFaces = octree->kNearestNeighbors(point, k);
    for (auto& tri : nearestFaces)
    {
        Vec3<T> closest = closestPointOnTriangle(tri.second, point);
        T dist = (point - closest).length();

        if (dist < minDist)
        {
            minDist = dist;
            closestPoint = closest;
        }
    }

    return minDist;
}

//

// Determine if point is inside mesh using ray casting
template <typename T>
bool SignedDistanceCalculator<T>::isPointInside(const Vec3<T>& point) const
{
    // std::cout << point.x << ", " << point.y << ", " << point.z << "\n"; exit(1);
    Vec3<T> rayDir(T(1), T(0), T(0));
    Ray<T> ray(point, rayDir);

    std::unordered_set<Point<T>, PointHash<T>> intersection_points;
    // std::unordered_set<Vec3<T>, Vec3TypeHash<T>> intersection_points;
    // intersection_points.clear();

    octree->traverse([&](const AABB<T>& bounds, int depth, bool isLeaf, const std::vector<Triangle<T>>& triangles) {

        if (triangles.empty()) { return; }

        if (!isLeaf) { return; }

        T tmin, tmax;
        if (!bounds.rayIntersect(ray, tmin, tmax)) { return; }

        for (const auto& tri : triangles)
        {
            Vec3<T> edge1 = tri.v1 - tri.v0;
            Vec3<T> edge2 = tri.v2 - tri.v0;
            Vec3<T> h = ray.direction.cross(edge2);
            T a = edge1.dot(h);

            if (std::abs(a) < T(1e-12)) continue;

            T f = T(1) / a;
            Vec3<T> s = ray.origin - tri.v0;
            T u = f * s.dot(h);

            if (u < T(0) || u > T(1)) continue;

            Vec3<T> q = s.cross(edge1);
            T v = f * ray.direction.dot(q);

            if (v < T(0) || u + v > T(1)) continue;

            T t = f * edge2.dot(q);

            if (t > T(1e-12)) {
                Point<T> point_intersect(ray.origin + ray.direction * t);
                intersection_points.insert(point_intersect);
            }
        }

        });


    size_t intersections = intersection_points.size();
    return (intersections % 2) == 1;
}

//

template <typename T>
void SignedDistanceCalculator<T>::rebuildOctree(const AABB<T>& bounds)
{
    // if (!triangles.empty())
    // {
    //     octree = std::make_unique<Octree<T>>(triangles);
    // }

    octree = std::make_unique<Octree<Triangle<T>, T>>(bounds);
}

//

// Add triangle to the surface
template <typename T>
void SignedDistanceCalculator<T>::addTriangle(const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2, int faceId)
{
    // triangles.emplace_back(v0, v1, v2);
    // octree.reset(); // Invalidate octree, will be rebuilt on next query
    // std::cout << "added...\n";
    octree->insert(Triangle<T>(v0, v1, v2, faceId));
}

//

template <typename T>
void SignedDistanceCalculator<T>::addTriangle(const Triangle<T>& triangle)
{
    octree->insert(triangle);
}

//

// Add triangles from vertex array and index array
template <typename T>
void SignedDistanceCalculator<T>::addTriangles(const std::vector<Vec3<T>>& vertices, const std::vector<std::vector<int>>& faces)
{
    int faceId = 0;
    for (const auto& face : faces)
    {
        if (face.size() >= 3)
        {
            addTriangle(vertices[face[0]], vertices[face[1]], vertices[face[2]], faceId);
            faceId++;
        }
    }
}

//

// Calculate signed distance
template <typename T>
T SignedDistanceCalculator<T>::signedDistance(const Vec3<T>& point) const
{
    // if (triangles.empty())
    // {
    //     return std::numeric_limits<T>::max();
    // }

    // Build octree if needed
    // if (!octree)
    // {
    //     const_cast<SignedDistanceCalculator*>(this)->rebuildOctree();
    // }

    Vec3<T> closestPoint;
    T distance = unsignedDistanceToSurface(point, closestPoint);
    // std::cout << distance << "\n"; exit(1);
    bool inside = isPointInside(point);

    return inside ? -distance : distance;
}

//

// Get closest point on surface
template <typename T>
Vec3<T> SignedDistanceCalculator<T>::getClosestPoint(const Vec3<T>& point) const
{
    // if (!octree) {
    //     const_cast<SignedDistanceCalculator*>(this)->rebuildOctree();
    // }

    Vec3<T> closestPoint;
    unsignedDistanceToSurface(point, closestPoint);
    return closestPoint;
}

//

template <typename T>
void SignedDistanceCalculator<T>::clear()
{
    // triangles.clear();
    // octree.reset();
}

//

template <typename T>
size_t SignedDistanceCalculator<T>::getTriangleCount() const
{
    // return triangles.size();
    auto triangles = octree->getAll();
    // for (auto& t : triangles) { std::cout << t.id << "\n"; }

    return triangles.size();
}

//

#ifdef TEST_SDC

int main()
{
    SignedDistanceCalculator<double> sdc;

    Vec3<double> a(0.0, 0.0, 0.0);
    Vec3<double> b(1.0, 0.0, 0.0);
    Vec3<double> c(0.0, 1.0, 0.0);
    Triangle<double> tri(a, b, c);
    Vec3<double> p(1, 0.2, 1.0);

    Vec3<double> closestPoint = sdc.closestPointOnTriangle(tri, p);
    std::cout << closestPoint.x << ", " << closestPoint.y << ", " << closestPoint.z << "\n";
    // exit(1);


    // Create a simple cube mesh
    std::vector<Vec3<double>> vertices = {
        Vec3<double>(-1, -1, -1), Vec3<double>(1, -1, -1), Vec3<double>(1,  1, -1), Vec3<double>(-1,  1, -1),
        Vec3<double>(-1, -1,  1), Vec3<double>(1, -1,  1), Vec3<double>(1,  1,  1), Vec3<double>(-1,  1,  1)
    };

    std::vector<std::vector<int>> faces = {
        {0, 1, 2}, {0, 2, 3}, {4, 6, 5}, {4, 7, 6},
        {0, 4, 5}, {0, 5, 1}, {2, 6, 7}, {2, 7, 3},
        {0, 3, 7}, {0, 7, 4}, {1, 5, 6}, {1, 6, 2}
    };

    AABB<double> bounds(Vec3<double>(-1.1, -1.1, -1.1), Vec3<double>(1.1, 1.1, 1.1));
    sdc.rebuildOctree(bounds);
    sdc.addTriangles(vertices, faces);
    // std::cout << sdc.getTriangleCount(); exit(1);

    // Test points
    std::vector<Vec3<double>> testPoints = {
        Vec3<double>(0, 0, 0),      // Inside cube
        Vec3<double>(-2, 0, 0),      // Outside cube
        Vec3<double>(1, 0, 0),      // On surface
        Vec3<double>(0.5, 0.5, 0.5) // Inside cube
    };

    printf("Testing with octree acceleration:\n");
    for (const auto& point : testPoints) {
        double dist = sdc.signedDistance(point);
        Vec3<double> closest = sdc.getClosestPoint(point);

        printf("Point (%.2f, %.2f, %.2f): signed distance = %.4f\n",
            point.x, point.y, point.z, dist);
        printf("  Closest point: (%.2f, %.2f, %.2f)\n",
            closest.x, closest.y, closest.z);
        printf("  %s\n\n", std::abs(dist) < 1e-12 ? "On" : (dist < 0 ? "Inside" : "Outside"));

    }

    return 0;
}

#endif

#ifdef TEST_SDC_1

//g++ -std=c++20 -DTEST_SDC_1 -Ofast -o sdc_test ../meshfree/meshfree/src/Geometry/SignedDistanceCalculator.cpp ../meshfree/meshfree/src/Geometry/Octree.cpp ../meshfree/meshfree/src/Geometry/GeometryIO.cpp ../meshfree/meshfree/src/Geometry/PointGenerator.cpp

int main()
{

    // read the geometry

    GeometryIO<float> geom;

    std::vector<Point<float>> nodes;
    std::vector<Triangle<float>> triangles;
    bool readNodes = true;
    geom.read_STL_binary(nodes, triangles, "utah_teapot.stl", readNodes);
    // std::cout << triangles.size() << "\n";

    // set up the sdc with bounds and octree

    SignedDistanceCalculator<float> sdc;

    AABB<float> bounds(Vec3<float>(-8.5, -5.5, -0.1), Vec3<float>(9.5, 5.6, 8.6));
    sdc.rebuildOctree(bounds);
    for (size_t i = 0; i < triangles.size(); ++i)
    {
        sdc.addTriangle(triangles[i]);
    }

    // generate Halton points and transform them to the bounding box of the teapot

    PointGenerator<float> pgf;

    size_t numpts = 50000;
    size_t ndim = 3;
    auto H = pgf.halton(numpts, ndim);
    // std::cout << H.size() << "\n";

    std::vector<std::vector<float>> Rot{ {18, 0, 0}, {0, 11, 0}, {0, 0, 9} };
    std::vector<float> shft{ -8.5, -5.5, 0 };
    for (auto& p : H)
    {
        p.position.transform(Rot, shft);
        // std::cout << p.to_string() << "\n";
    }

    // calculate the signed distance and remove Halton points outside the teapot

    std::vector<Point<float>> points_inside;
    std::vector<float> dist_inside;
    for (auto& p : H)
    {
        float dist = sdc.signedDistance(p.position);
        if (dist <= 0)
        {
            // std::cout << p.to_string() << ", " << dist << "\n";
            points_inside.push_back(p);
            dist_inside.push_back(dist);
        }
    }

    // concatenate points on the surface to the output

    for (auto& p : nodes)
    {
        // std::cout << p.to_string() << ", " << 0.0f << "\n";
        points_inside.push_back(p);
        dist_inside.push_back(0);
    }

    // std::cout << points_inside.size() << "\n";

    // reduce point density where absolute values of signed distance from boundary are large

    float hmin = 2;
    float hmax = 4;
    float scale = 1.2;

    auto points_remain = pgf.reject(points_inside, dist_inside, { hmin, hmax, scale });

    for (size_t i = 0; i < points_remain.first.size(); ++i)
    {
        std::cout << points_remain.first[i].to_string() << ", " << points_remain.second[i] << "\n";
    }

    return 0;
}

#endif