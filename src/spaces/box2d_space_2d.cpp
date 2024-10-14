#include "box2d_space_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../box2d_project_settings.h"
#include "../type_conversions.h"
#include <godot_cpp/variant/utility_functions.hpp>

Box2DSpace2D::Box2DSpace2D() {
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2{ 0.0, 3.0 * 980.0 };
	world_id = b2CreateWorld(&world_def);
}

Box2DSpace2D::~Box2DSpace2D() {
	if (direct_state) {
		memdelete(direct_state);
	}
	b2DestroyWorld(world_id);
}

void Box2DSpace2D::step(float p_step) {
	int substeps = Box2DProjectSettings::get_substeps();
	b2World_Step(world_id, p_step, substeps);

	last_step = p_step;
}

void Box2DSpace2D::sync_state() {
	body_events = b2World_GetBodyEvents(world_id);

	for (int i = 0; i < body_events.moveCount; ++i) {
		const b2BodyMoveEvent *event = body_events.moveEvents + i;
		Box2DBody2D *body = static_cast<Box2DBody2D *>(event->userData);
		ERR_FAIL_COND(!body);
		body->sync_state(event->transform, event->fellAsleep);
	}
}

Box2DPhysicsDirectSpaceState2D *Box2DSpace2D::get_direct_state() {
	if (!direct_state) {
		direct_state = memnew(Box2DPhysicsDirectSpaceState2D(this));
	}
	return direct_state;
}