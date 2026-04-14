cmake_minimum_required(VERSION 3.10)

# Define the project name
project(AsciiCube)

# Use C++17 for modern standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the executable name and the source file
add_executable(rotate main.cpp)

# Link the math library (m) to handle sin() and cos()
target_link_libraries(rotate m)