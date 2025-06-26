#include "ego_actor.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>

using namespace godot;

void EgoActor::_bind_methods() {
    // No methods to bind for now
}

EgoActor::EgoActor() {
    UtilityFunctions::print("EgoActor constructor called!");
    
    // Create a visual representation
    Ref<PlaneMesh> plane_mesh = memnew(PlaneMesh);
    plane_mesh->set_size(Vector2(2.0, 2.0)); // 2x2 unit plane
    
    // Create a material to make it visible
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_albedo(Color(0.2, 0.8, 0.2, 1.0)); // Green color
    
    // Create the mesh instance
    MeshInstance3D* mesh_instance = memnew(MeshInstance3D);
    mesh_instance->set_mesh(plane_mesh);
    mesh_instance->set_material_override(material);
    
    // Add it as a child
    add_child(mesh_instance);
}

EgoActor::~EgoActor() {
    UtilityFunctions::print("EgoActor destructor called!");
} 