cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(elevator_simulation VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SFML package
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Add source files
add_executable(elevator_simulation 
    src/main.cpp
    src/Elevator.cpp
    src/Request.cpp
    src/Building.cpp
    src/Floor.cpp
    src/GUI.cpp  # Add the new GUI implementation file
)

# Link SFML libraries
target_link_libraries(elevator_simulation sfml-graphics sfml-window sfml-system sfml-audio)

# Include directories
target_include_directories(elevator_simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)