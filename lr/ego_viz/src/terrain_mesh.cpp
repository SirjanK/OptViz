#include "terrain_mesh.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/color.hpp>
#include <cfloat>

using namespace godot;

void TerrainMesh::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_terrain_data", "metadata_json_path", "binary_z_path"), &TerrainMesh::load_terrain_data);
    ClassDB::bind_method(D_METHOD("generate_mesh"), &TerrainMesh::generate_mesh);
    ClassDB::bind_method(D_METHOD("set_material_color", "color"), &TerrainMesh::set_material_color);
    ClassDB::bind_method(D_METHOD("get_height_at", "x", "y"), &TerrainMesh::get_height_at);
}

TerrainMesh::TerrainMesh() {
    UtilityFunctions::print("TerrainMesh constructor called!");
    grid_width = 0;
    grid_height = 0;
    min_x = 0.0f;
    min_y = 0.0f;
    delta = 0.0f;
}

TerrainMesh::~TerrainMesh() {
    UtilityFunctions::print("TerrainMesh destructor called!");
}

void TerrainMesh::_ready() {
    UtilityFunctions::print("TerrainMesh _ready() called!");
    
    // Load terrain data and generate mesh
    load_terrain_data("res://assets/mock_terrain_metadata.json", "res://assets/mock_terrain.bin");
    generate_mesh();
}

void TerrainMesh::load_terrain_data(const String& metadata_json_path, const String& binary_z_path) {
    // Clear the terrain points
    heightmap.clear();
    
    Ref<FileAccess> file = FileAccess::open(metadata_json_path, FileAccess::READ);
    if (!file.is_valid()) {
        UtilityFunctions::print("Failed to open metadata file: " + metadata_json_path);
        return;
    }

    // Read entire JSON file into string
    String json_str = file->get_as_text();
    file->close();

    // Parse JSON string into Dictionary
    Variant parsed = JSON::parse_string(json_str);
    if (parsed.get_type() != Variant::DICTIONARY) {
        UtilityFunctions::print("Failed to parse JSON metadata");
        return;
    }
    
    Dictionary metadata = parsed;
    
    // Extract metadata into instance variables with proper type conversion
    min_x = (float)metadata["x_min"];
    grid_width = (int)metadata["width"];
    min_y = (float)metadata["y_min"];
    grid_height = (int)metadata["height"];
    delta = (float)metadata["delta"];
    
    UtilityFunctions::print("Parsed terrain metadata:");
    UtilityFunctions::print("  x_min: " + String::num(min_x));
    UtilityFunctions::print("  width: " + String::num_int64(grid_width));
    UtilityFunctions::print("  y_min: " + String::num(min_y));
    UtilityFunctions::print("  height: " + String::num_int64(grid_height));
    UtilityFunctions::print("  delta: " + String::num(delta));

    // Create 2D heightmap grid
    heightmap.clear();
    heightmap.resize(grid_height);
    
    for (int y = 0; y < grid_height; y++) {
        Array row;
        row.resize(grid_width);
        heightmap[y] = row;
    }
    
    // Load binary height data
    Ref<FileAccess> bin_file = FileAccess::open(binary_z_path, FileAccess::READ);
    if (!bin_file.is_valid()) {
        UtilityFunctions::print("Failed to open binary file: " + binary_z_path);
        return;
    }
    
    // Read height values as float32
    for (int y = 0; y < grid_height; y++) {
        Array row = heightmap[y];
        for (int x = 0; x < grid_width; x++) {
            float height = bin_file->get_float();
            row[x] = height;
        }
        heightmap[y] = row;
    }
    
    bin_file->close();
    
    UtilityFunctions::print("Loaded heightmap: " + String::num_int64(grid_width) + "x" + String::num_int64(grid_height));
}

void TerrainMesh::generate_mesh() {
    if (grid_width < 2 || grid_height < 2) {
        UtilityFunctions::print("Grid too small to generate mesh");
        return;
    }
    
    Ref<SurfaceTool> surface_tool = memnew(SurfaceTool);
    surface_tool->begin(Mesh::PRIMITIVE_TRIANGLES);
    
    // Calculate height range for color mapping
    float min_height = FLT_MAX;
    float max_height = -FLT_MAX;
    
    for (int y = 0; y < grid_height; y++) {
        Array row = heightmap[y];
        for (int x = 0; x < grid_width; x++) {
            float height = row[x];
            if (height < min_height) min_height = height;
            if (height > max_height) max_height = height;
        }
    }
    
    float height_range = max_height - min_height;
    UtilityFunctions::print("Height range: " + String::num(min_height) + " to " + String::num(max_height));
    
    // Generate triangles for each grid cell
    for (int y = 0; y < grid_height - 1; y++) {
        Array row1 = heightmap[y];
        Array row2 = heightmap[y + 1];
        
        for (int x = 0; x < grid_width - 1; x++) {
            // Calculate world coordinates
            float x1 = min_x + x * delta;
            float x2 = min_x + (x + 1) * delta;
            float y1 = min_y + y * delta;
            float y2 = min_y + (y + 1) * delta;
            
            // Get heights at the four corners
            float z1 = row1[x];      // bottom-left
            float z2 = row1[x + 1];  // bottom-right
            float z3 = row2[x];      // top-left
            float z4 = row2[x + 1];  // top-right
            
            // Create vertices
            Vector3 p1(x1, y1, z1);
            Vector3 p2(x2, y1, z2);
            Vector3 p3(x1, y2, z3);
            Vector3 p4(x2, y2, z4);
            
            // Calculate colors based on height
            Color c1 = get_height_color(z1, min_height, height_range);
            Color c2 = get_height_color(z2, min_height, height_range);
            Color c3 = get_height_color(z3, min_height, height_range);
            Color c4 = get_height_color(z4, min_height, height_range);
            
            // Create two triangles for this grid cell
            // Triangle 1: p1, p2, p3
            surface_tool->add_vertex(p1);
            surface_tool->set_color(c1);
            surface_tool->add_vertex(p2);
            surface_tool->set_color(c2);
            surface_tool->add_vertex(p3);
            surface_tool->set_color(c3);
            
            // Triangle 2: p2, p4, p3
            surface_tool->add_vertex(p2);
            surface_tool->set_color(c2);
            surface_tool->add_vertex(p4);
            surface_tool->set_color(c4);
            surface_tool->add_vertex(p3);
            surface_tool->set_color(c3);
        }
    }
    
    Ref<ArrayMesh> terrain_mesh = surface_tool->commit();
    
    // Create material with vertex colors enabled
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_albedo(Color(1.0, 1.0, 1.0, 1.0)); // White base color
    material->set_roughness(0.8);
    
    // Set the mesh and material
    set_mesh(terrain_mesh);
    set_material_override(material);
    
    UtilityFunctions::print("Terrain mesh generated with " + String::num_int64((grid_width - 1) * (grid_height - 1) * 2) + " triangles");
    UtilityFunctions::print("Height-based coloring applied: blue(low) -> green(medium) -> brown(high) -> white(peaks)");
}

void TerrainMesh::set_material_color(const Color& color) {
    Ref<StandardMaterial3D> material = get_material_override();
    if (material.is_valid()) {
        material->set_albedo(color);
    }
}

float TerrainMesh::get_height_at(float x, float y) const {
    if (grid_width < 2 || grid_height < 2) return 0.0f;

    // Convert world coordinates to grid coordinates
    int grid_x = (int)((x - min_x) / delta);
    int grid_y = (int)((y - min_y) / delta);

    // Clamp to grid bounds
    grid_x = Math::clamp(grid_x, 0, grid_width - 1);
    grid_y = Math::clamp(grid_y, 0, grid_height - 1);

    Array row = heightmap[grid_y];
    return row[grid_x];
}

Color TerrainMesh::get_height_color(float height, float min_height, float height_range) const {
    if (height_range <= 0.0f) {
        return Color(0.5, 0.5, 0.5, 1.0); // Gray if no height variation
    }
    
    // Normalize height to 0-1 range
    float normalized_height = (height - min_height) / height_range;
    normalized_height = Math::clamp(normalized_height, 0.0f, 1.0f);
    
    // Create a color gradient: blue -> green -> brown -> white
    Color color;
    if (normalized_height < 0.25f) {
        // Blue to green (water to grass)
        float t = normalized_height / 0.25f;
        color = Color(0.0, 0.2 + 0.6 * t, 0.8 - 0.4 * t, 1.0);
    } else if (normalized_height < 0.6f) {
        // Green to brown (grass to earth)
        float t = (normalized_height - 0.25f) / 0.35f;
        color = Color(0.2 + 0.4 * t, 0.8 - 0.3 * t, 0.4 - 0.2 * t, 1.0);
    } else if (normalized_height < 0.85f) {
        // Brown to light brown (earth to rock)
        float t = (normalized_height - 0.6f) / 0.25f;
        color = Color(0.6 + 0.2 * t, 0.5 - 0.1 * t, 0.2 + 0.1 * t, 1.0);
    } else {
        // Light brown to white (rock to snow)
        float t = (normalized_height - 0.85f) / 0.15f;
        color = Color(0.8 + 0.2 * t, 0.4 + 0.6 * t, 0.3 + 0.7 * t, 1.0);
    }
    
    return color;
} 