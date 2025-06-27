#include "terrain_mesh.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/color.hpp>
#include <cfloat>

using namespace godot;

void TerrainMesh::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_terrain_data", "csv_path"), &TerrainMesh::load_terrain_data);
    ClassDB::bind_method(D_METHOD("load_terrain_binary", "bin_path"), &TerrainMesh::load_terrain_binary);
    ClassDB::bind_method(D_METHOD("generate_mesh"), &TerrainMesh::generate_mesh);
    ClassDB::bind_method(D_METHOD("set_material_color", "color"), &TerrainMesh::set_material_color);
    ClassDB::bind_method(D_METHOD("get_min_bounds"), &TerrainMesh::get_min_bounds);
    ClassDB::bind_method(D_METHOD("get_max_bounds"), &TerrainMesh::get_max_bounds);
    ClassDB::bind_method(D_METHOD("get_grid_width"), &TerrainMesh::get_grid_width);
    ClassDB::bind_method(D_METHOD("get_grid_height"), &TerrainMesh::get_grid_height);
    ClassDB::bind_method(D_METHOD("get_height_at", "x", "y"), &TerrainMesh::get_height_at);
}

TerrainMesh::TerrainMesh() {
    UtilityFunctions::print("TerrainMesh constructor called!");
    grid_width = 0;
    grid_height = 0;
    min_bounds = Vector3(0, 0, 0);
    max_bounds = Vector3(0, 0, 0);
}

TerrainMesh::~TerrainMesh() {
    UtilityFunctions::print("TerrainMesh destructor called!");
}

void TerrainMesh::_ready() {
    UtilityFunctions::print("TerrainMesh _ready() called!");
    
    // Load terrain data and generate mesh
    load_terrain_data("res://assets/mock_terrain.csv");
    generate_mesh();
}

void TerrainMesh::load_terrain_data(const String& csv_path) {
    // Clear the terrain points
    terrain_points.clear();
    
    Ref<FileAccess> file = FileAccess::open(csv_path, FileAccess::READ);
    if (!file.is_valid()) {
        UtilityFunctions::print("Failed to open terrain file: " + csv_path);
        return;
    }
    
    // Skip header
    String header = file->get_line();
    UtilityFunctions::print("Header: " + header);
    
    // Read all data points
    while (!file->eof_reached()) {
        String line = file->get_line();
        if (line.is_empty()) continue;
        
        PackedStringArray parts = line.split(",");
        if (parts.size() >= 3) {
            Vector3 point(parts[0].to_float(), parts[1].to_float(), parts[2].to_float());
            terrain_points.append(point);
        }
    }
    
    UtilityFunctions::print("Loaded " + String::num_int64(terrain_points.size()) + " terrain points");
}

void TerrainMesh::load_terrain_binary(const String& bin_path) {
    // Implementation of load_terrain_binary method
}

void TerrainMesh::generate_mesh() {
    // TODO(sirjan) don't hardcode
    if (terrain_points.size() < 32) {
        UtilityFunctions::print("Grid too small to generate mesh");
        return;
    }
    
    Ref<SurfaceTool> surface_tool = memnew(SurfaceTool);
    surface_tool->begin(Mesh::PRIMITIVE_TRIANGLES);
    
    // Calculate height range for color mapping
    // TODO(sirjan) don't hardcode
    float min_height = -0.1f;
    float max_height = 6.0f;
    float height_range = max_height - min_height;
    
    UtilityFunctions::print("Height range: " + String::num(min_height) + " to " + String::num(max_height));
    
    // Generate triangles for each grid cell
    for (int i = 0; i < terrain_points.size() - 1; i++) {
        Vector3 p1 = terrain_points[i];
        Vector3 p2 = terrain_points[i + 1];
        Vector3 p3 = terrain_points[i + grid_width];
        Vector3 p4 = terrain_points[i + grid_width + 1];
        
        // Calculate colors based on height
        Color c1 = get_height_color(p1.z, min_height, height_range);
        Color c2 = get_height_color(p2.z, min_height, height_range);
        Color c3 = get_height_color(p3.z, min_height, height_range);
        Color c4 = get_height_color(p4.z, min_height, height_range);
        
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
    
    Ref<ArrayMesh> terrain_mesh = surface_tool->commit();
    
    // Create material with vertex colors enabled
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_albedo(Color(1.0, 1.0, 1.0, 1.0)); // White base color
    material->set_roughness(0.8);
    // Note: Vertex colors should work automatically with white base color
    
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

    // Find i0, i1 (x indices)
    int i0 = 0;
    Array row0_first = terrain_points[0];
    Vector3 v00 = row0_first[0];
    float x0 = v00.x;
    float x1 = x0;
    for (int i = 0; i < grid_width - 1; ++i) {
        Array row0 = terrain_points[i];
        Array row1 = terrain_points[i+1];
        Vector3 v0 = row0[0];
        Vector3 v1 = row1[0];
        if (x >= v0.x && x <= v1.x) {
            i0 = i;
            x0 = v0.x;
            x1 = v1.x;
            break;
        }
    }
    int i1 = i0 + 1;
    if (i1 >= grid_width) i1 = grid_width - 1;

    // Find j0, j1 (y indices)
    int j0 = 0;
    Array col0_first = terrain_points[0];
    Vector3 v0y = col0_first[0];
    float y0 = v0y.y;
    float y1 = y0;
    for (int j = 0; j < grid_height - 1; ++j) {
        Array col0 = terrain_points[0];
        Vector3 v0 = col0[j];
        Vector3 v1 = col0[j+1];
        if (y >= v0.y && y <= v1.y) {
            j0 = j;
            y0 = v0.y;
            y1 = v1.y;
            break;
        }
    }
    int j1 = j0 + 1;
    if (j1 >= grid_height) j1 = grid_height - 1;

    // Get the four corners
    Array row_i0 = terrain_points[i0];
    Array row_i1 = terrain_points[i1];
    Vector3 p00 = row_i0[j0];
    Vector3 p10 = row_i1[j0];
    Vector3 p01 = row_i0[j1];
    Vector3 p11 = row_i1[j1];
    float z00 = p00.z;
    float z10 = p10.z;
    float z01 = p01.z;
    float z11 = p11.z;

    // Bilinear interpolation
    float tx = (x1 - x0) > 0.0f ? (x - x0) / (x1 - x0) : 0.0f;
    float ty = (y1 - y0) > 0.0f ? (y - y0) / (y1 - y0) : 0.0f;
    float z0 = z00 * (1 - tx) + z10 * tx;
    float z1 = z01 * (1 - tx) + z11 * tx;
    float z = z0 * (1 - ty) + z1 * ty;
    return z;
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