#!/bin/bash

# Build script for STL Dashboard Backend (Production)
echo "Building STL Dashboard Backend for production..."

# Navigate to backend directory
cd "$(dirname "$0")"

# Create necessary directories
echo "Creating upload directories..."
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

# Compile the C++ executable with optimization
echo "Compiling generate_ou executable..."
g++ -std=c++17 -O3 -Wall \
    generate_ou.cpp \
    SignedDistanceCalculator.cpp \
    PointGenerator.cpp \
    -o bin/generate_ou

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ C++ compilation successful!"
    chmod +x bin/generate_ou
    echo "✅ Executable permissions set"
else
    echo "❌ C++ compilation failed!"
    exit 1
fi

cd ..

# Install Python dependencies
echo "Installing Python dependencies..."
pip install -r requirements.txt

echo "✅ Build completed successfully!"
echo "🚀 Ready for deployment!" 