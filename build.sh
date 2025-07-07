#!/bin/bash

# Build script for STL Dashboard Backend
echo "Building STL Dashboard Backend..."

# Navigate to backend directory
cd "$(dirname "$0")"

# Create necessary directories
mkdir -p uploads/stl
mkdir -p uploads/ou
mkdir -p cpp/bin

# Compile C++ code
echo "Compiling C++ backend..."
cd cpp

# Check if g++ is available
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found. Please install g++."
    exit 1
fi

# Compile the C++ executable
g++ -std=c++17 -O2 \
    generate_ou.cpp \
    SignedDistanceCalculator.cpp \
    PointGenerator.cpp \
    -o bin/generate_ou

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "C++ compilation successful!"
    chmod +x bin/generate_ou
else
    echo "C++ compilation failed!"
    exit 1
fi

cd ..

echo "Build completed successfully!" 