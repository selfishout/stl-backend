#include <iostream>
#include <vector>
#include "Point.hpp"
#include "SignedDistanceCalculator.hpp"
#include "GeometryIO.hpp"

using namespace MF;

int main() {
    // Create a simple cube for testing
    std::vector<Vec3<double>> vertices = {
        Vec3<double>(0, 0, 0),  // 0
        Vec3<double>(1, 0, 0),  // 1
        Vec3<double>(1, 1, 0),  // 2
        Vec3<double>(0, 1, 0),  // 3
        Vec3<double>(0, 0, 1),  // 4
        Vec3<double>(1, 0, 1),  // 5
        Vec3<double>(1, 1, 1),  // 6
        Vec3<double>(0, 1, 1)   // 7
    };
    
    std::vector<std::vector<int>> faces = {
        {0, 1, 2}, {0, 2, 3}, // bottom
        {4, 7, 6}, {4, 6, 5}, // top
        {0, 4, 5}, {0, 5, 1}, // front
        {2, 6, 7}, {2, 7, 3}, // back
        {0, 3, 7}, {0, 7, 4}, // left
        {1, 5, 6}, {1, 6, 2}  // right
    };
    
    SignedDistanceCalculator<double> sdf;
    
    // Build octree FIRST
    AABB<double> bounds(Vec3<double>(0, 0, 0), Vec3<double>(1, 1, 1));
    sdf.rebuildOctree(bounds);
    
    // THEN add triangles to SDF calculator
    for (const auto& face : faces) {
        if (face.size() >= 3) {
            sdf.addTriangle(vertices[face[0]], vertices[face[1]], vertices[face[2]], 0);
        }
    }
    
    // Print triangle count in octree
    std::cout << "Triangle count in octree: " << sdf.getTriangleCount() << std::endl;
    
    // Test points
    std::vector<Vec3<double>> testPoints = {
        Vec3<double>(0.5, 0.5, 0.5),  // Should be inside
        Vec3<double>(0.1, 0.1, 0.1),  // Should be inside
        Vec3<double>(0.9, 0.9, 0.9),  // Should be inside
        Vec3<double>(1.5, 0.5, 0.5),  // Should be outside
        Vec3<double>(-0.1, 0.5, 0.5), // Should be outside
        Vec3<double>(0.5, 1.5, 0.5),  // Should be outside
    };
    
    for (size_t i = 0; i < testPoints.size(); ++i) {
        bool isInside = sdf.isPointInside(testPoints[i]);
        std::cout << "Point " << i << " (" << testPoints[i].x << ", " 
                  << testPoints[i].y << ", " << testPoints[i].z << "): " 
                  << (isInside ? "INSIDE" : "OUTSIDE") << std::endl;
    }
    
    return 0;
} 