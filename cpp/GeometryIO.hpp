#pragma once

#include "Point.hpp"
#include <fstream>
#include <vector>
#include <string>

namespace MF
{
    template<typename T>
    class GeometryIO
    {
    public:
        static bool readSTL(const std::string& filename, std::vector<Vec3<T>>& vertices, std::vector<std::vector<int>>& faces)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
                return false;
                
            // Read STL header (80 bytes)
            char header[80];
            file.read(header, 80);
            
            // Check if it's binary STL
            bool isBinary = true;
            for (int i = 0; i < 80; ++i)
            {
                if (header[i] < 0 || header[i] > 127)
                {
                    isBinary = false;
                    break;
                }
            }
            
            if (isBinary)
                return readSTLBinary(file, vertices, faces);
            else
                return readSTLAscii(file, vertices, faces);
        }
        
    private:
        static bool readSTLBinary(std::ifstream& file, std::vector<Vec3<T>>& vertices, std::vector<std::vector<int>>& faces)
        {
            // Read triangle count
            uint32_t triangleCount;
            file.read(reinterpret_cast<char*>(&triangleCount), 4);
            
            vertices.clear();
            faces.clear();
            
            for (uint32_t i = 0; i < triangleCount; ++i)
            {
                // Read normal (12 bytes)
                float normal[3];
                file.read(reinterpret_cast<char*>(normal), 12);
                
                // Read vertices (36 bytes)
                float v[9];
                file.read(reinterpret_cast<char*>(v), 36);
                
                // Read attribute byte count (2 bytes)
                uint16_t attributeCount;
                file.read(reinterpret_cast<char*>(&attributeCount), 2);
                
                // Add vertices
                size_t baseIndex = vertices.size();
                vertices.push_back(Vec3<T>(v[0], v[1], v[2]));
                vertices.push_back(Vec3<T>(v[3], v[4], v[5]));
                vertices.push_back(Vec3<T>(v[6], v[7], v[8]));
                
                // Add face
                faces.push_back({static_cast<int>(baseIndex), static_cast<int>(baseIndex + 1), static_cast<int>(baseIndex + 2)});
            }
            
            return true;
        }
        
        static bool readSTLAscii(std::ifstream& file, std::vector<Vec3<T>>& vertices, std::vector<std::vector<int>>& faces)
        {
            // Reset file position
            file.seekg(0);
            
            std::string line;
            vertices.clear();
            faces.clear();
            
            while (std::getline(file, line))
            {
                if (line.find("vertex") != std::string::npos)
                {
                    T x, y, z;
                    if (sscanf(line.c_str(), "vertex %f %f %f", &x, &y, &z) == 3)
                    {
                        vertices.push_back(Vec3<T>(x, y, z));
                    }
                }
            }
            
            // Create faces (assuming triangles are stored as 3 consecutive vertices)
            for (size_t i = 0; i < vertices.size(); i += 3)
            {
                if (i + 2 < vertices.size())
                {
                    faces.push_back({static_cast<int>(i), static_cast<int>(i + 1), static_cast<int>(i + 2)});
                }
            }
            
            return true;
        }
    };
} 