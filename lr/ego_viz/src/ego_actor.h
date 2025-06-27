#ifndef EGO_ACTOR_H
#define EGO_ACTOR_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class EgoActor : public Node3D {
    GDCLASS(EgoActor, Node3D)

private:
    Array trajectory_data;
    int current_frame;
    Vector3 camera_offset;
    float fps;
    float time_since_last_frame;
    bool is_playing;

protected:
    static void _bind_methods();

public:
    EgoActor();
    ~EgoActor();
    
    void _ready() override;
    void _process(double delta) override;
    
    void load_trajectory_data(const String& csv_path);
    void set_frame(int frame);
    int get_current_frame() const { return current_frame; }
    int get_total_frames() const { return trajectory_data.size(); }
    
    // Playback control
    void set_fps(float new_fps);
    float get_fps() const;
    void play();
    void pause();
    void stop();
    bool is_playing_animation() const;
};

}

#endif // EGO_ACTOR_H 