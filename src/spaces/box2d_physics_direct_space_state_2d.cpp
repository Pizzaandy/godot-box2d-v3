
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../type_conversions.h"

void Box2DPhysicsDirectSpaceState2D::_bind_methods() {}

struct ShapeCollector {
	int max_results = 0;
	Vector<b2ShapeId> shapes;
};

bool overlap_callback(b2ShapeId shapeId, void *context) {
	ShapeCollector *collector = static_cast<ShapeCollector *>(context);
	collector->shapes.push_back(shapeId);
	return collector->shapes.size() < collector->max_results;
}

int32_t Box2DPhysicsDirectSpaceState2D::_intersect_point(
		const Vector2 &p_position,
		uint64_t p_canvas_instance_id,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeResult *p_results,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, 0);
	b2WorldId world = space->get_world_id();

	if (p_max_results <= 0) {
		return 0;
	}

	b2Transform transform = b2Transform_identity;
	transform.p = to_box2d(p_position);

	ShapeCollector collector;
	collector.max_results = p_max_results;
	b2QueryFilter filter = b2DefaultQueryFilter();
	b2World_OverlapPoint(world, to_box2d(Vector2()), transform, filter, overlap_callback, &collector);

	int result_count = collector.shapes.size();

	for (int i = 0; i < result_count; i++) {
		b2BodyId body_id = b2Shape_GetBody(collector.shapes[i]);
		Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
		ERR_FAIL_COND_V(!body, 0);

		Box2DBody2D::Shape *shape = static_cast<Box2DBody2D::Shape *>(b2Shape_GetUserData(collector.shapes[i]));
		ERR_FAIL_COND_V(!shape, 0);
		ERR_FAIL_COND_V(shape->index < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_results++;

		result.shape = shape->index;
		result.rid = body->get_rid();
		result.collider_id = body->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return result_count;
}
