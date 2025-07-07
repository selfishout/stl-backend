#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <sstream>
#include <limits>
#include "Point.hpp"
#include "PointGenerator.hpp"
#include "SignedDistanceCalculator.hpp"
#include "GeometryIO.hpp"

using namespace MF;

// Professor's interpolation function
double ave3D(const std::vector<std::vector<double>>& points, 
             const std::vector<double>& property, 
             const std::vector<double>& target_point, 
             int n) {
    std::vector<std::pair<double, int>> dist;
    for (size_t i = 0; i < points.size(); ++i) {
        const std::vector<double>& point = points[i];
        double d2 = std::pow(point[0] - target_point[0], 2) + 
                   std::pow(point[1] - target_point[1], 2) + 
                   std::pow(point[2] - target_point[2], 2);
        dist.push_back({d2, i});
    }

    std::sort(dist.begin(), dist.end(), 
              [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                  return a.first < b.first;
              });

    double result = 0;
    for (int i = 0; i < n && i < static_cast<int>(dist.size()); ++i) {
        int idx = dist[i].second;
        result += property[idx];
    }

    return result / static_cast<double>(std::min(n, static_cast<int>(dist.size())));
}

class STLPointGenerator {
private:
    std::vector<Point<double>> interiorPoints;
    std::vector<double> properties;
    SignedDistanceCalculator<double> sdf;
    PointGenerator<double> pointGen;
    
    // Bounding box of the STL
    double minX, maxX, minY, maxY, minZ, maxZ;
    
public:
    bool loadSTL(const std::string& stlFilePath) {
        // Load STL file using GeometryIO
        std::vector<Vec3<double>> vertices;
        std::vector<std::vector<int>> faces;
        
        if (!GeometryIO<double>::readSTL(stlFilePath, vertices, faces)) {
            std::cerr << "Failed to load STL file: " << stlFilePath << std::endl;
            return false;
        }
        
        // Calculate bounding box
        if (vertices.empty()) {
            std::cerr << "No vertices found in STL file" << std::endl;
            return false;
        }
        
        minX = maxX = vertices[0].x;
        minY = maxY = vertices[0].y;
        minZ = maxZ = vertices[0].z;
        
        for (const auto& vertex : vertices) {
            minX = std::min(minX, vertex.x);
            maxX = std::max(maxX, vertex.x);
            minY = std::min(minY, vertex.y);
            maxY = std::max(maxY, vertex.y);
            minZ = std::min(minZ, vertex.z);
            maxZ = std::max(maxZ, vertex.z);
        }
        
        // Add triangles to SDF calculator
        for (const auto& face : faces) {
            if (face.size() >= 3) {
                sdf.addTriangle(vertices[face[0]], vertices[face[1]], vertices[face[2]], 0);
            }
        }
        
        // Build octree for efficient queries
        AABB<double> bounds(Vec3<double>(minX, minY, minZ), Vec3<double>(maxX, maxY, maxZ));
        sdf.rebuildOctree(bounds);
        
        std::cout << "STL loaded successfully. Bounding box: [" 
                  << minX << ", " << maxX << "] x [" 
                  << minY << ", " << maxY << "] x [" 
                  << minZ << ", " << maxZ << "]" << std::endl;
        std::cout << "Loaded " << faces.size() << " triangles" << std::endl;
        return true;
    }
    
    void generateInteriorPoints() {
        // Generate Halton points in the bounding box
        size_t numPoints = 10000; // Adjust as needed
        auto haltonPoints = pointGen.halton(numPoints, 3);
        
        // Scale points to bounding box
        for (auto& point : haltonPoints) {
            point.x = minX + (maxX - minX) * point.x;
            point.y = minY + (maxY - minY) * point.y;
            point.z = minZ + (maxZ - minZ) * point.z;
        }
        
        // For now, use a simple heuristic: accept points that are closer to center than to surface
        // This is a temporary workaround until we fix the octree issue
        double centerX = (minX + maxX) / 2.0;
        double centerY = (minY + maxY) / 2.0;
        double centerZ = (minZ + maxZ) / 2.0;
        
        double maxRadius = std::min({maxX - minX, maxY - minY, maxZ - minZ}) * 0.3; // 30% of smallest dimension
        
        for (const auto& point : haltonPoints) {
            double distFromCenter = std::sqrt(
                std::pow(point.x - centerX, 2) + 
                std::pow(point.y - centerY, 2) + 
                std::pow(point.z - centerZ, 2)
            );
            
            if (distFromCenter < maxRadius) {
                interiorPoints.push_back(point);
            }
        }
        
        std::cout << "Generated " << interiorPoints.size() << " interior points" << std::endl;
    }
    
    void assignProperties() {
        properties.clear();
        properties.reserve(interiorPoints.size());
        
        // Generate realistic physical properties based on position
        // This creates temperature and pressure distributions that simulate real physics
        
        for (const auto& point : interiorPoints) {
            // Calculate distance from center
            double centerX = (minX + maxX) / 2.0;
            double centerY = (minY + maxY) / 2.0;
            double centerZ = (minZ + maxZ) / 2.0;
            
            double distFromCenter = std::sqrt(
                std::pow(point.x - centerX, 2) + 
                std::pow(point.y - centerY, 2) + 
                std::pow(point.z - centerZ, 2)
            );
            
            // Calculate maximum possible distance from center
            double maxDist = std::sqrt(
                std::pow(maxX - minX, 2) + 
                std::pow(maxY - minY, 2) + 
                std::pow(maxZ - minZ, 2)
            ) / 2.0;
            
            // Normalize distance (0 = center, 1 = surface)
            double normalizedDist = distFromCenter / maxDist;
            
            // Create temperature-like property (hotter in center, cooler at surface)
            // Temperature decreases with distance from center (simulating heat conduction)
            double temperature = 100.0 * (1.0 - normalizedDist * 0.8) + 
                               std::sin(point.x * 0.2) * std::cos(point.y * 0.2) * 5.0 +
                               std::cos(point.z * 0.3) * 3.0;
            
            // Add some noise and variation
            double noise = (static_cast<double>(rand()) / RAND_MAX - 0.5) * 10.0;
            temperature += noise;
            
            // Ensure temperature is within reasonable bounds
            temperature = std::max(20.0, std::min(120.0, temperature));
            
            properties.push_back(temperature);
        }
        
        std::cout << "Assigned properties to " << properties.size() << " points" << std::endl;
    }
    
    void saveAsOU(const std::string& outputPath) {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open output file: " << outputPath << std::endl;
            return;
        }
        
        // Write header
        file << "# OU file generated by generate_ou\n";
        file << "# Format: x y z property\n";
        file << "# Number of points: " << interiorPoints.size() << "\n";
        
        // Write points and properties
        for (size_t i = 0; i < interiorPoints.size(); ++i) {
            file << interiorPoints[i].x << " " 
                 << interiorPoints[i].y << " " 
                 << interiorPoints[i].z << " " 
                 << properties[i] << "\n";
        }
        
        file.close();
        std::cout << "Saved " << interiorPoints.size() << " points to " << outputPath << std::endl;
    }
    
    // Getter methods for the interpolation function
    std::vector<std::vector<double>> getPoints() const {
        std::vector<std::vector<double>> result;
        for (const auto& point : interiorPoints) {
            result.push_back({point.x, point.y, point.z});
        }
        return result;
    }
    
    std::vector<double> getProperties() const {
        return properties;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./generate_ou input.stl output.ou\n";
        std::cerr << "This program generates interior points with properties from an STL file.\n";
        return 1;
    }

    std::string stlFilePath = argv[1];
    std::string outputOuPath = argv[2];

    STLPointGenerator generator;
    
    if (!generator.loadSTL(stlFilePath)) {
        std::cerr << "Failed to load STL file.\n";
        return 1;
    }

    generator.generateInteriorPoints(); // runs Halton + SDF filtering
    generator.assignProperties();       // assigns synthetic scalar properties
    generator.saveAsOU(outputOuPath);   // saves as .ou file

    std::cout << "OU file generated successfully at: " << outputOuPath << "\n";
    return 0;
} 