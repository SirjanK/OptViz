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
    Ref<FileAccess> file = FileAccess::open(csv_path, FileAccess::READ);
    if (!file.is_valid()) {
        UtilityFunctions::print("Failed to open terrain file: " + csv_path);
        return;
    }
    
    // Skip header
    String header = file->get_line();
    UtilityFunctions::print("Header: " + header);
    
    // Read all data points
    Array points;
    while (!file->eof_reached()) {
        String line = file->get_line();
        if (line.is_empty()) continue;
        
        PackedStringArray parts = line.split(",");
        if (parts.size() >= 3) {
            Vector3 point(parts[0].to_float(), parts[1].to_float(), parts[2].to_float());
            points.append(point);
        }
    }
    
    // Determine grid dimensions by finding unique X and Y values
    Array unique_x, unique_y;
    for (int i = 0; i < points.size(); i++) {
        Vector3 point = points[i];
        if (!unique_x.has(point.x)) {
            unique_x.append(point.x);
        }
        if (!unique_y.has(point.y)) {
            unique_y.append(point.y);
        }
    }
    
    // Sort unique values
    unique_x.sort();
    unique_y.sort();
    
    grid_width = unique_x.size();
    grid_height = unique_y.size();
    
    UtilityFunctions::print("Grid dimensions: " + String::num_int64(grid_width) + " x " + String::num_int64(grid_height));
    
    // Create 2D grid of points
    terrain_points.clear();
    terrain_points.resize(grid_width);
    
    for (int x = 0; x < grid_width; x++) {
        Array row;
        row.resize(grid_height);
        terrain_points[x] = row;
    }
    
    // Fill the grid
    for (int i = 0; i < points.size(); i++) {
        Vector3 point = points[i];
        
        // Find grid indices
        int x_idx = unique_x.find(point.x);
        int y_idx = unique_y.find(point.y);
        
        if (x_idx >= 0 && y_idx >= 0 && x_idx < grid_width && y_idx < grid_height) {
            Array row = terrain_points[x_idx];
            row[y_idx] = point;
            terrain_points[x_idx] = row;
        }
    }
    
    // Calculate bounds
    min_bounds = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    max_bounds = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    
    for (int x = 0; x < grid_width; x++) {
        Array row = terrain_points[x];
        for (int y = 0; y < grid_height; y++) {
            Vector3 point = row[y];
            if (point.x < min_bounds.x) min_bounds.x = point.x;
            if (point.y < min_bounds.y) min_bounds.y = point.y;
            if (point.z < min_bounds.z) min_bounds.z = point.z;
            if (point.x > max_bounds.x) max_bounds.x = point.x;
            if (point.y > max_bounds.y) max_bounds.y = point.y;
            if (point.z > max_bounds.z) max_bounds.z = point.z;
        }
    }
    
    UtilityFunctions::print("Loaded " + String::num_int64(points.size()) + " terrain points");
    UtilityFunctions::print("Bounds: min(" + String::num(min_bounds.x) + ", " + String::num(min_bounds.y) + ", " + String::num(min_bounds.z) + 
                           ") max(" + String::num(max_bounds.x) + ", " + String::num(max_bounds.y) + ", " + String::num(max_bounds.z) + ")");
}

void TerrainMesh::generate_mesh() {
    if (grid_width < 2 || grid_height < 2) {
        UtilityFunctions::print("Grid too small to generate mesh");
        return;
    }
    
    Ref<SurfaceTool> surface_tool = memnew(SurfaceTool);
    surface_tool->begin(Mesh::PRIMITIVE_TRIANGLES);
    
    // Generate triangles for each grid cell
    for (int x = 0; x < grid_width - 1; x++) {
        for (int y = 0; y < grid_height - 1; y++) {
            Array row1 = terrain_points[x];
            Array row2 = terrain_points[x + 1];
            
            Vector3 p1 = row1[y];           // Current point
            Vector3 p2 = row1[y + 1];       // Point to the right
            Vector3 p3 = row2[y];           // Point below
            Vector3 p4 = row2[y + 1];       // Point below and to the right
            
            // Create two triangles for this grid cell
            // Triangle 1: p1, p2, p3
            surface_tool->add_vertex(p1);
            surface_tool->add_vertex(p2);
            surface_tool->add_vertex(p3);
            
            // Triangle 2: p2, p4, p3
            surface_tool->add_vertex(p2);
            surface_tool->add_vertex(p4);
            surface_tool->add_vertex(p3);
        }
    }
    
    Ref<ArrayMesh> terrain_mesh = surface_tool->commit();
    
    // Create material
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_albedo(Color(0.4, 0.3, 0.2, 1.0)); // Brown/earth color
    material->set_roughness(0.9);
    
    // Set the mesh and material
    set_mesh(terrain_mesh);
    set_material_override(material);
    
    UtilityFunctions::print("Terrain mesh generated with " + String::num_int64((grid_width - 1) * (grid_height - 1) * 2) + " triangles");
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