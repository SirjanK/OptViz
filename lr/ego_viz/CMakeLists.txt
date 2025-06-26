cmake_minimum_required(VERSION 3.22)
project(ego_viz)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add godot-cpp as a subdirectory
add_subdirectory(godot-cpp)

# Define the extension library
add_library(ego_viz SHARED
    src/register_types.cpp
    src/ego_actor.cpp
)

# Link against godot-cpp
target_link_libraries(ego_viz PRIVATE godot::cpp)

# Set output directory
set_target_properties(ego_viz PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/godot"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/godot"
) 