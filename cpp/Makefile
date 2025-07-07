CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
INCLUDES = -I.
LIBS = 

# Source files
SOURCES = generate_ou.cpp PointGenerator.cpp SignedDistanceCalculator.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = bin/generate_ou

# Default target
all: $(TARGET)

# Create bin directory if it doesn't exist
$(TARGET): $(OBJECTS) | bin
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)

bin:
	mkdir -p bin

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -f *.o $(TARGET)

# Install executable to system path (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Test the executable
test: $(TARGET)
	@echo "Testing generate_ou..."
	@if [ -f "../uploads/stl/utah_teapot.stl" ]; then \
		./$(TARGET) ../uploads/stl/utah_teapot.stl test_output.ou; \
		echo "Test completed. Check test_output.ou"; \
	else \
		echo "No test STL file found. Please place an STL file in ../uploads/stl/"; \
	fi

.PHONY: all clean install test 