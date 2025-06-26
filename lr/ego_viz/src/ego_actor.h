#ifndef EGO_ACTOR_H
#define EGO_ACTOR_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>

namespace godot {

class EgoActor : public Node3D {
    GDCLASS(EgoActor, Node3D)

private:
    MeshInstance3D* mesh_instance;
    PlaneMesh* plane_mesh;

protected:
    static void _bind_methods();

public:
    EgoActor();
    ~EgoActor();

    void _ready() override;
    void create_surface();
};

}

#endif // EGO_ACTOR_H 