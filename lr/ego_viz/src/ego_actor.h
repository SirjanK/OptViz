#ifndef EGO_ACTOR_H
#define EGO_ACTOR_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class EgoActor : public Node3D {
    GDCLASS(EgoActor, Node3D)

public:
    EgoActor();
    ~EgoActor();
    
    void _input(const Ref<InputEvent>& event) override;
    void _ready() override;
    
    void load_trajectory_data(const String& csv_path);
    void set_frame(int frame);
    int get_current_frame() const { return current_frame; }
    int get_total_frames() const { return trajectory_data.size(); }

protected:
    static void _bind_methods();

private:
    Array trajectory_data;  // Array of position data
    int current_frame;
    Vector3 camera_offset;  // Fixed offset behind the actor
    
    void update_position();
    void update_camera_position();
};

}

#endif // EGO_ACTOR_H 