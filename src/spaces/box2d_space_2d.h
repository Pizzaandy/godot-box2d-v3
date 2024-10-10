#pragma once

#include "box2d/box2d.h"
#include <godot_cpp/variant/rid.hpp>
#include "box2d_physics_direct_space_state_2d.h"

using namespace godot;

class Box2DPhysicsDirectSpaceState2D;

class Box2DSpace2D {
public:
	explicit Box2DSpace2D();

	~Box2DSpace2D();

	void step(float p_step);

	RID get_rid() const { return rid; }

	void set_rid(const RID &p_rid) { rid = p_rid; }

	void call_queries();

	Box2DPhysicsDirectSpaceState2D *get_direct_state();

private:
	b2WorldId world_id;
	RID rid;
	Box2DPhysicsDirectSpaceState2D *direct_state = nullptr;
};