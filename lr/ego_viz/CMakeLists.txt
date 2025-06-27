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
    src/terrain_mesh.cpp
)

# Add include directories for godot-cpp headers
target_include_directories(ego_viz PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/godot-cpp/gen/include
)

# Link against godot-cpp
target_link_libraries(ego_viz PRIVATE godot::cpp)

# Set output directory
set_target_properties(ego_viz PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/godot"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/godot"
)

# On macOS, create a framework
if(APPLE)
    set_target_properties(ego_viz PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION A
        MACOSX_FRAMEWORK_IDENTIFIER "com.example.ego_viz"
        MACOSX_FRAMEWORK_BUNDLE_VERSION "1.0"
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING "1.0"
        OUTPUT_NAME "libego_viz"
    )
endif() 