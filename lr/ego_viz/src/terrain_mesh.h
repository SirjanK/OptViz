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
    Array terrain_points;
    int grid_width;
    int grid_height;
    Vector3 min_bounds;
    Vector3 max_bounds;

protected:
    static void _bind_methods();

public:
    TerrainMesh();
    ~TerrainMesh();

    void _ready() override;
    
    void load_terrain_data(const String& csv_path);
    void generate_mesh();
    void set_material_color(const Color& color);
    
    Vector3 get_min_bounds() const { return min_bounds; }
    Vector3 get_max_bounds() const { return max_bounds; }
    int get_grid_width() const { return grid_width; }
    int get_grid_height() const { return grid_height; }
    float get_height_at(float x, float y) const;
};

}

#endif // TERRAIN_MESH_H 