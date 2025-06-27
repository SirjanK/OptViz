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
    // all points of the terrain mesh (x, y, z)
    Array terrain_points;

protected:
    static void _bind_methods();

public:
    TerrainMesh();
    ~TerrainMesh();

    void _ready() override;
    
    // Load terrain data from the CSV file and store in terrain_points
    void load_terrain_data(const String& csv_path);
    // Load terrain data from binary file
    void load_terrain_binary(const String& bin_path);
    // Generate the mesh from the terrain data
    void generate_mesh();
    // Set the material color of the terrain mesh
    void set_material_color(const Color& color);
    
    float get_height_at(float x, float y) const;
    Color get_height_color(float height, float min_height, float height_range) const;
};

}

#endif // TERRAIN_MESH_H 