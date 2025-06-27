#include "ego_actor.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

void EgoActor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_trajectory_data", "csv_path"), &EgoActor::load_trajectory_data);
    ClassDB::bind_method(D_METHOD("set_frame", "frame"), &EgoActor::set_frame);
    ClassDB::bind_method(D_METHOD("get_current_frame"), &EgoActor::get_current_frame);
    ClassDB::bind_method(D_METHOD("get_total_frames"), &EgoActor::get_total_frames);
    ClassDB::bind_method(D_METHOD("set_fps", "fps"), &EgoActor::set_fps);
    ClassDB::bind_method(D_METHOD("get_fps"), &EgoActor::get_fps);
    ClassDB::bind_method(D_METHOD("play"), &EgoActor::play);
    ClassDB::bind_method(D_METHOD("pause"), &EgoActor::pause);
    ClassDB::bind_method(D_METHOD("stop"), &EgoActor::stop);
    ClassDB::bind_method(D_METHOD("is_playing_animation"), &EgoActor::is_playing_animation);
}

EgoActor::EgoActor() {
    UtilityFunctions::print("EgoActor constructor called!");
    current_frame = 0;
    camera_offset = Vector3(0, 2, 5);  // 2 units above, 5 units behind
    fps = 10.0f;  // 10 FPS default
    time_since_last_frame = 0.0f;
    is_playing = true;  // Start playing automatically
}

EgoActor::~EgoActor() {
    UtilityFunctions::print("EgoActor destructor called!");
}

void EgoActor::_ready() {
    UtilityFunctions::print("EgoActor _ready() called!");
    
    // Create a triangle mesh using SurfaceTool
    Ref<SurfaceTool> surface_tool = memnew(SurfaceTool);
    surface_tool->begin(Mesh::PRIMITIVE_TRIANGLES);
    
    // Define triangle vertices (pointing forward) - make it larger
    surface_tool->add_vertex(Vector3(-1, 0, -1));  // Back left
    surface_tool->add_vertex(Vector3(1, 0, -1));   // Back right
    surface_tool->add_vertex(Vector3(0, 0, 1));    // Front center (point)
    
    Ref<ArrayMesh> triangle_mesh = surface_tool->commit();

    // Create a material to make it visible
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_albedo(Color(1.0, 0.0, 0.0, 1.0)); // Bright red color
    material->set_emission(Color(0.5, 0.0, 0.0, 1.0)); // Add emission to make it glow
    material->set_emission_energy_multiplier(3.0); // Make emission brighter

    // Create the mesh instance
    MeshInstance3D* mesh_instance = memnew(MeshInstance3D);
    mesh_instance->set_mesh(triangle_mesh);
    mesh_instance->set_material_override(material);

    // Add it as a child
    add_child(mesh_instance);
    UtilityFunctions::print("Triangle mesh added to EgoActor");

    // Create and add camera
    Camera3D* camera = memnew(Camera3D);
    camera->set_name("FollowCamera");
    camera->set_current(true);  // Make this the active camera
    camera->set_near(0.1);  // Set near plane to avoid clipping
    camera->set_far(1000.0);  // Set far plane
    add_child(camera);
    UtilityFunctions::print("Camera added to EgoActor");

    // Load trajectory data
    load_trajectory_data("res://assets/trajectory.csv");

    // Set initial position if data loaded
    if (trajectory_data.size() > 0) {
        set_frame(0);
        UtilityFunctions::print("Initial frame set to 0");
    } else {
        UtilityFunctions::print("No trajectory data loaded!");
    }
}

void EgoActor::_process(double delta) {
    if (!is_playing || trajectory_data.size() == 0) {
        return;
    }
    
    time_since_last_frame += delta;
    float frame_time = 1.0f / fps;
    
    if (time_since_last_frame >= frame_time) {
        time_since_last_frame -= frame_time;
        
        // Move to next frame
        int next_frame = current_frame + 1;
        if (next_frame >= trajectory_data.size()) {
            // Loop back to start
            next_frame = 0;
        }
        set_frame(next_frame);
    }
}

void EgoActor::load_trajectory_data(const String& csv_path) {
    Ref<FileAccess> file = FileAccess::open(csv_path, FileAccess::READ);
    if (!file.is_valid()) {
        UtilityFunctions::print("Failed to open trajectory file: " + csv_path);
        return;
    }
    
    // Skip header
    String header = file->get_line();
    
    // Read data
    while (!file->eof_reached()) {
        String line = file->get_line();
        if (line.is_empty()) continue;
        
        PackedStringArray parts = line.split(",");
        if (parts.size() >= 4) {
            Array frame_data;
            frame_data.append(parts[0].to_int());  // t
            frame_data.append(parts[1].to_float()); // x
            frame_data.append(parts[2].to_float()); // y
            frame_data.append(parts[3].to_float()); // z
            trajectory_data.append(frame_data);
        }
    }
    
    UtilityFunctions::print("Loaded " + String::num_int64(trajectory_data.size()) + " trajectory frames");
}

void EgoActor::set_frame(int frame) {
    if (frame < 0 || frame >= trajectory_data.size()) {
        return;
    }
    
    current_frame = frame;
    
    // Update position
    Array frame_data = trajectory_data[frame];
    Vector3 position(frame_data[1], frame_data[2], frame_data[3]);
    set_position(position);
    
    // Calculate steering direction (where we're heading)
    Vector3 direction = Vector3(0, 0, 1); // Default forward direction
    
    if (frame < trajectory_data.size() - 1) {
        // Get next position to calculate direction
        Array next_frame_data = trajectory_data[frame + 1];
        Vector3 next_position(next_frame_data[1], next_frame_data[2], next_frame_data[3]);
        direction = (next_position - position).normalized();
    } else if (frame > 0) {
        // If at last frame, use direction from previous frame
        Array prev_frame_data = trajectory_data[frame - 1];
        Vector3 prev_position(prev_frame_data[1], prev_frame_data[2], prev_frame_data[3]);
        direction = (position - prev_position).normalized();
    }
    
    // Calculate rotation to face the direction
    if (direction.length() > 0.001) { // Avoid division by zero
        // Create a rotation that points the triangle's forward direction (Z-axis) toward the movement direction
        Vector3 up = Vector3(0, 1, 0);
        Vector3 forward = Vector3(0, 0, 1); // Triangle's forward direction
        
        // Create a basis (rotation matrix) that aligns forward with direction
        Vector3 right = direction.cross(up).normalized();
        up = right.cross(direction).normalized(); // Recalculate up to ensure orthogonality
        
        Basis rotation_basis(right, up, direction);
        Quaternion rotation = rotation_basis.get_rotation_quaternion();
        
        set_quaternion(rotation);
    }
    
    // Update camera position to follow behind
    Camera3D* camera = Object::cast_to<Camera3D>(get_node_or_null("FollowCamera"));
    if (camera) {
        // Calculate camera position in world coordinates, behind the actor
        Vector3 camera_pos = position - camera_offset;
        camera->set_global_position(camera_pos);
        camera->look_at(position, Vector3(0, 1, 0));
    }
    
    // Print frame info every 10 frames to avoid spam
    if (frame % 10 == 0) {
        UtilityFunctions::print("Frame " + String::num_int64(frame) + 
                               ": pos(" + String::num(position.x) + ", " + 
                               String::num(position.y) + ", " + 
                               String::num(position.z) + ")");
    }
}

void EgoActor::set_fps(float new_fps) {
    fps = new_fps;
}

float EgoActor::get_fps() const {
    return fps;
}

void EgoActor::play() {
    is_playing = true;
}

void EgoActor::pause() {
    is_playing = false;
}

void EgoActor::stop() {
    is_playing = false;
    current_frame = 0;
    time_since_last_frame = 0.0f;
    if (trajectory_data.size() > 0) {
        set_frame(0);
    }
}

bool EgoActor::is_playing_animation() const {
    return is_playing;
} 