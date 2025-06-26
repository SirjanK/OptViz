#include "ego_actor.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void EgoActor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_surface"), &EgoActor::create_surface);
}

EgoActor::EgoActor() {
    mesh_instance = nullptr;
    plane_mesh = nullptr;
}

EgoActor::~EgoActor() {
    // Cleanup will be handled by Godot's reference counting
}

void EgoActor::_ready() {
    create_surface();
}

void EgoActor::create_surface() {
    // Create a plane mesh
    plane_mesh = memnew(PlaneMesh);
    plane_mesh->set_size(Vector2(10.0, 10.0)); // 10x10 unit plane
    
    // Create mesh instance
    mesh_instance = memnew(MeshInstance3D);
    mesh_instance->set_mesh(plane_mesh);
    
    // Add to scene
    add_child(mesh_instance);
    
    // Position the plane at origin
    mesh_instance->set_position(Vector3(0, 0, 0));
} 