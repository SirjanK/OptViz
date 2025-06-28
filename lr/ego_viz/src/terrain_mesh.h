#ifndef TERRAIN_MESH_H
#define TERRAIN_MESH_H

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class TerrainMesh : public MeshInstance3D {
    GDCLASS(TerrainMesh, MeshInstance3D)

private:
    // terrain metadata from JSON
    float min_x;     // minimum x coordinate
    float min_y;     // minimum y coordinate
    int grid_width;  // width of terrain grid
    int grid_height; // height of terrain grid
    float delta;     // spacing between grid points

    // heightmap grid
    Array heightmap;  // 2D grid of heights

protected:
    static void _bind_methods();

public:
    TerrainMesh();
    ~TerrainMesh();

    void _ready() override;
    
    // Load terrain data from the CSV file and store in terrain_points
    void load_terrain_data(const String& metadata_json_path, const String& binary_z_path);
    // Generate the mesh from the terrain data
    void generate_mesh();
    // Set the material color of the terrain mesh
    void set_material_color(const Color& color);
    
    float get_height_at(float x, float y) const;
    Color get_height_color(float height, float min_height, float height_range) const;
};

}

#endif // TERRAIN_MESH_H 