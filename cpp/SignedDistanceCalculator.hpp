#pragma once

#include "Octree.hpp"
#include "PointGenerator.hpp"
#include "GeometryIO.hpp"

#include <unordered_set>

namespace MF
{
    template<typename T>
    class SignedDistanceCalculator
    {
    private:

        std::vector<Triangle<T>> triangles;
        std::unique_ptr<Octree<Triangle<T>, T>> octree;

    public:

        SignedDistanceCalculator() = default;

        // Function to find the closest point on a line segment to a given point
        Vec3<T> closestPointOnSegment(const Vec3<T>& p, const Vec3<T>& a, const Vec3<T>& b);

        // Calculate closest point on triangle to query point
        Vec3<T> closestPointOnTriangle(const Triangle<T>& tri, const Vec3<T>& p) const;

        // Calculate unsigned distance to surface using octree acceleration
        T unsignedDistanceToSurface(const Vec3<T>& point, Vec3<T>& closestPoint) const;

        // Determine if point is inside mesh using ray casting
        bool isPointInside(const Vec3<T>& point) const;

        void rebuildOctree(const AABB<T>& bounds);

        // Add triangle to the surface
        void addTriangle(const Vec3<T>& v0, const Vec3<T>& v1, const Vec3<T>& v2, int faceId);

        void addTriangle(const Triangle<T>& triangle);

        // Add triangles from vertex array and index array
        void addTriangles(const std::vector<Vec3<T>>& vertices, const std::vector<std::vector<int>>& faces);

        // Calculate signed distance
        T signedDistance(const Vec3<T>& point) const;

        // Get closest point on surface
        Vec3<T> getClosestPoint(const Vec3<T>& point) const;

        void clear();

        size_t getTriangleCount() const;

    };
}