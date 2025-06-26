#ifndef EGO_ACTOR_H
#define EGO_ACTOR_H

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

class EgoActor : public Node3D {
    GDCLASS(EgoActor, Node3D)

protected:
    static void _bind_methods();

public:
    EgoActor();
    ~EgoActor();
};

}

#endif // EGO_ACTOR_H 