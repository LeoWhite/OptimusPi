cmake_minimum_required (VERSION 2.8)
project (OptimusPi)

# We want all the 'bin' files to output into a specific directory
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# The version number
set (OptimusPi_VERSION_MAJOR 1)
set (OptimusPi_VERSION_MINOR 0)

# Configure header file to pass the CMake settings to the
# source code
configure_file (
 "${PROJECT_SOURCE_DIR}/OptimusPiConfig.h.in"
 "${PROJECT_BINARY_DIR}/OptimusPiConfig.h"
)

# Add the binary tree for the search path
include_directories("${PROJECT_BINARY_DIR}")

# The PiWars library
include_directories("${PROJECT_SOURCE_DIR}/PiWars")
add_subdirectory(PiWars)

# The main executable
add_executable(OptimusPi main.cpp)
target_link_libraries(OptimusPi PiWars)

