#include <iostream>
#include <string>
#include "PointGenerator.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./generate_ou input.stl output.ou\n";
        return 1;
    }

    std::string stlFilePath = argv[1];
    std::string outputOuPath = argv[2];

    PointGenerator generator;
    if (!generator.loadSTL(stlFilePath)) {
        std::cerr << "Failed to load STL file.\n";
        return 1;
    }

    generator.generateInteriorPoints(); // runs Halton + SDF filtering
    generator.assignProperties();       // assigns synthetic scalar properties
    generator.saveAsOU(outputOuPath);   // saves as .ou file

    std::cout << "OU file generated at: " << outputOuPath << "\n";
    return 0;
}
