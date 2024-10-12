#include "box2d_space_2d.h"
#include "../bodies/box2d_body_2d.h"

Box2DSpace2D::Box2DSpace2D() {
	b2WorldDef world_def = b2DefaultWorldDef();
	world_def.gravity = b2Vec2{ 0.0, 980.0 };
	world_id = b2CreateWorld(&world_def);
}

Box2DSpace2D::~Box2DSpace2D() {
	b2DestroyWorld(world_id);
}

void Box2DSpace2D::step(float p_step) {
	b2World_Step(world_id, p_step, 4);

	last_step = p_step;

	b2BodyEvents events = b2World_GetBodyEvents(world_id);
	for (int i = 0; i < events.moveCount; ++i) {
		const b2BodyMoveEvent *event = events.moveEvents + i;
		Box2DBody2D *body = static_cast<Box2DBody2D *>(event->userData);
		body->update_state(event->transform, event->fellAsleep);
	}
}

void Box2DSpace2D::call_queries() {}

Box2DPhysicsDirectSpaceState2D *Box2DSpace2D::get_direct_state() {
	if (direct_state == nullptr) {
		direct_state = memnew(Box2DPhysicsDirectSpaceState2D(this));
	}
	return direct_state;
}

float Box2DSpace2D::get_last_step() {
	return last_step;
}