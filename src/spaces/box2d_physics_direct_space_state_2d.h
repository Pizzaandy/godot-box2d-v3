#pragma once

#include <godot_cpp/classes/physics_direct_space_state2d_extension.hpp>
#include "box2d_space_2d.h"

using namespace godot;

class Box2DSpace2D;

class Box2DPhysicsDirectSpaceState2D : public PhysicsDirectSpaceState2DExtension {
	GDCLASS(Box2DPhysicsDirectSpaceState2D, PhysicsDirectSpaceState2DExtension);

public:
	Box2DPhysicsDirectSpaceState2D() = default;
	explicit Box2DPhysicsDirectSpaceState2D(Box2DSpace2D *p_space);

private:
	static void _bind_methods();
	Box2DSpace2D *space = nullptr;
};