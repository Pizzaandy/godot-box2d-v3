
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
//#include "../shapes/box2d_shape_instance.h"

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
	b2QueryFilter filter = b2DefaultQueryFilter();

	if (p_max_results <= 0) {
		return 0;
	}

	b2Transform transform = b2Transform_identity;
	transform.p = to_box2d(p_position);

	ShapeCollector collector{ p_max_results };

	b2World_OverlapPoint(world, to_box2d(Vector2()), transform, filter, overlap_callback, &collector);

	int result_count = collector.shapes.size();

	for (int i = 0; i < result_count; i++) {
		b2BodyId body_id = b2Shape_GetBody(collector.shapes[i]);
		Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
		Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(collector.shapes[i]));
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

struct RaycastHit {
	b2ShapeId shapeId;
	b2Vec2 point;
	b2Vec2 normal;
	bool hit = false;
};

float raycast_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
	RaycastHit *result = static_cast<RaycastHit *>(context);
	result->shapeId = shapeId;
	result->point = point;
	result->normal = normal;
	result->hit = true;
	return fraction;
}

bool Box2DPhysicsDirectSpaceState2D::_intersect_ray(
		const Vector2 &p_from,
		const Vector2 &p_to,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		bool p_hit_from_inside,
		PhysicsServer2DExtensionRayResult *p_result) {
	ERR_FAIL_COND_V(!space, false);

	b2WorldId world = space->get_world_id();
	b2QueryFilter filter = b2DefaultQueryFilter();

	if (p_hit_from_inside) {
		b2Transform transform = b2Transform_identity;
		transform.p = to_box2d(p_from);
		ShapeCollector collector{ 1 };

		b2World_OverlapPoint(world, to_box2d(Vector2()), transform, filter, overlap_callback, &collector);

		int overlap_count = collector.shapes.size();
		if (overlap_count > 0) {
			b2ShapeId shape_id = collector.shapes[0];
			b2BodyId body_id = b2Shape_GetBody(shape_id);
			Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
			Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(shape_id));
			ERR_FAIL_COND_V(shape->index < 0, 0);

			p_result->position = p_from;
			p_result->normal = Vector2();
			p_result->shape = shape->index;
			p_result->rid = body->get_rid();
			p_result->collider_id = body->get_instance_id();
			if (p_result->collider_id.is_valid()) {
				p_result->collider = get_instance_hack(p_result->collider_id);
			}
			return true;
		}
	}

	RaycastHit raycast_result;
	b2World_CastRay(world, to_box2d(p_from), to_box2d(p_to - p_from), filter, raycast_callback, &raycast_result);

	if (!raycast_result.hit) {
		return false;
	}

	b2BodyId body_id = b2Shape_GetBody(raycast_result.shapeId);
	Box2DBody2D *body = static_cast<Box2DBody2D *>(b2Body_GetUserData(body_id));
	Box2DShapeInstance *shape = static_cast<Box2DShapeInstance *>(b2Shape_GetUserData(raycast_result.shapeId));
	ERR_FAIL_COND_V(shape->index < 0, 0);

	PhysicsServer2DExtensionRayResult &result = *p_result;

	result.position = to_godot(raycast_result.point);
	result.normal = to_godot(raycast_result.normal).normalized();
	result.shape = shape->index;
	result.rid = body->get_rid();
	result.collider_id = body->get_instance_id();
	if (result.collider_id.is_valid()) {
		result.collider = get_instance_hack(result.collider_id);
	}

	return true;
}

// struct RaycastHitCollector {
// 	int max_results = 0;
// 	Vector<RaycastHit> hits;
// };

// float raycast_all_callback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context) {
// 	RaycastHitCollector *collector = static_cast<RaycastHitCollector *>(context);

// 	collector->hits.push_back(RaycastHit{ shapeId, point, normal });

// 	if (collector->hits.size() < collector->max_results) {
// 		return 0;
// 	}

// 	return 1;
// }