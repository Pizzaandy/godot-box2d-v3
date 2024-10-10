#include "box2d_space_2d.h"
#include <godot_cpp/variant/utility_functions.hpp>

Box2DSpace2D::Box2DSpace2D() {
	b2WorldDef worldDef = b2DefaultWorldDef();
	world_id = b2CreateWorld(&worldDef);
	UtilityFunctions::print("Hello from the physics world!");
}

Box2DSpace2D::~Box2DSpace2D() {
	b2DestroyWorld(world_id);
}

void Box2DSpace2D::step(float p_step) {
	b2World_Step(world_id, p_step, 4);
}

void Box2DSpace2D::call_queries() {}

Box2DPhysicsDirectSpaceState2D *Box2DSpace2D::get_direct_state() {
	if (direct_state == nullptr) {
		direct_state = memnew(Box2DPhysicsDirectSpaceState2D(this));
	}
	return direct_state;
}