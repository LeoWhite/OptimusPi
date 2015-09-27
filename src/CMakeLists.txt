cmake_minimum_required (VERSION 2.8)
project (OptimusPi)

# We want all the 'bin' files to output into a specific directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# We're going to be using C++11 features
add_definitions(-std=c++11)

# The version number
set (OptimusPi_VERSION_MAJOR 0)
set (OptimusPi_VERSION_MINOR 1)

# Configure header file to pass the CMake settings to the
# source code
configure_file (
 "${PROJECT_SOURCE_DIR}/OptimusPiConfig.h.in"
 "${PROJECT_BINARY_DIR}/OptimusPiConfig.h"
)

# Add the binary tree for the search path
include_directories("${PROJECT_BINARY_DIR}")
include_directories("/usr/include/libevdev-1.0/libevdev/")


# The PiWars library
include_directories("${PROJECT_SOURCE_DIR}/PiWars")
add_subdirectory(PiWars)

# The main executable
add_executable(OptimusPi main.cpp)
target_link_libraries(OptimusPi PiWars)

