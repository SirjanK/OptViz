cmake_minimum_required(VERSION 3.16)
project(terrain_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(test_terrain test_terrain.cpp)

# Set output directory
set_target_properties(test_terrain PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add compiler flags
if(APPLE)
    target_compile_options(test_terrain PRIVATE -Wall -Wextra)
elseif(UNIX)
    target_compile_options(test_terrain PRIVATE -Wall -Wextra)
elseif(WIN32)
    target_compile_options(test_terrain PRIVATE /W4)
endif() 