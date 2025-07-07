#include <iostream>
#include <vector>
#include "Point.hpp"
#include "SignedDistanceCalculator.hpp"

using namespace MF;

int main() {
    // Create a simple triangle
    Vec3<double> a(0.0, 0.0, 0.0);
    Vec3<double> b(1.0, 0.0, 0.0);
    Vec3<double> c(0.0, 1.0, 0.0);
    Triangle<double> tri(a, b, c);
    
    // Test point inside triangle
    Vec3<double> testPoint(0.3, 0.3, 0.0);
    
    std::cout << "Testing triangle: (" << a.x << "," << a.y << "," << a.z << ") "
              << "(" << b.x << "," << b.y << "," << b.z << ") "
              << "(" << c.x << "," << c.y << "," << c.z << ")" << std::endl;
    
    std::cout << "Test point: (" << testPoint.x << "," << testPoint.y << "," << testPoint.z << ")" << std::endl;
    
    // Test closest point calculation
    SignedDistanceCalculator<double> sdf;
    Vec3<double> closestPoint = sdf.closestPointOnTriangle(tri, testPoint);
    std::cout << "Closest point on triangle: (" << closestPoint.x << "," << closestPoint.y << "," << closestPoint.z << ")" << std::endl;
    
    // Test distance calculation
    double distance = (testPoint - closestPoint).length();
    std::cout << "Distance to triangle: " << distance << std::endl;
    
    return 0;
} 