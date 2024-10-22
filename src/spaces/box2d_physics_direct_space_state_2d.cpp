
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../servers/box2d_physics_server_2d.h"
#include "box2d_query_collectors.h"
#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>

void Box2DPhysicsDirectSpaceState2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("cast_shape"), &Box2DPhysicsDirectSpaceState2D::cast_shape, "parameters");
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
	b2QueryFilter filter = make_filter(p_collision_mask);

	if (p_max_results <= 0) {
		return 0;
	}

	b2Transform transform = b2Transform_identity;
	transform.p = to_box2d(p_position);

	ShapeOverlapCollector collector(p_max_results, QueryFilter(this));

	b2World_OverlapPoint(world, to_box2d(Vector2()), transform, filter, overlap_callback, &collector);

	for (ShapeOverlap overlap : collector.shapes) {
		ERR_FAIL_COND_V(overlap.shape->index < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_results++;

		result.shape = overlap.shape->index;
		result.rid = overlap.body->get_rid();
		result.collider_id = overlap.body->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return collector.shapes.size();
}

bool Box2DPhysicsDirectSpaceState2D::_intersect_ray(
		const Vector2 &p_from,
		const Vector2 &p_to,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		bool p_hit_from_inside,
		PhysicsServer2DExtensionRayResult *p_result) {
	ERR_FAIL_COND_V(!space, 0);

	b2WorldId world = space->get_world_id();
	b2QueryFilter filter = make_filter(p_collision_mask);

	if (p_hit_from_inside) {
		ShapeOverlapCollector collector(1, QueryFilter(this));
		b2World_OverlapPoint(world, to_box2d(p_from), b2Transform_identity, filter, overlap_callback, &collector);

		if (collector.shapes.size() > 0) {
			ShapeOverlap overlap = collector.shapes[0];
			ERR_FAIL_COND_V(overlap.shape->index < 0, 0);

			p_result->position = p_from;
			p_result->normal = Vector2();
			p_result->shape = overlap.shape->index;
			p_result->rid = overlap.body->get_rid();
			p_result->collider_id = overlap.body->get_instance_id();
			if (p_result->collider_id.is_valid()) {
				p_result->collider = get_instance_hack(p_result->collider_id);
			}
			return true;
		}
	}

	NearestCastHitCollector collector(QueryFilter(this));
	b2World_CastRay(world, to_box2d(p_from), to_box2d(p_to - p_from), filter, nearest_cast_callback, &collector);

	if (!collector.hit) {
		return false;
	}

	CastHit hit = collector.nearest_hit;

	ERR_FAIL_COND_V(hit.shape->index < 0, 0);

	PhysicsServer2DExtensionRayResult &result = *p_result;

	result.position = to_godot(hit.point);
	result.normal = to_godot(hit.normal).normalized();
	result.shape = hit.shape->index;
	result.rid = hit.body->get_rid();
	result.collider_id = hit.body->get_instance_id();
	if (result.collider_id.is_valid()) {
		result.collider = get_instance_hack(result.collider_id);
	}

	return true;
}

int32_t Box2DPhysicsDirectSpaceState2D::_intersect_shape(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeResult *p_result,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, 0);

	b2WorldId world = space->get_world_id();
	b2QueryFilter filter = make_filter(p_collision_mask);

	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, 0);

	CastHitCollector collector(p_max_results, QueryFilter(this));
	shape->cast_shape(world, p_transform, p_motion, filter, cast_callback, &collector);

	for (CastHit hit : collector.hits) {
		ERR_FAIL_COND_V(hit.shape->index < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_result++;

		result.shape = hit.shape->index;
		result.rid = hit.body->get_rid();
		result.collider_id = hit.body->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return collector.hits.size();
}

bool Box2DPhysicsDirectSpaceState2D::_cast_motion(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		float *p_closest_safe,
		float *p_closest_unsafe) {
	ERR_FAIL_COND_V(!space, false);

	b2WorldId world = space->get_world_id();
	b2QueryFilter filter = make_filter(p_collision_mask);

	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, false);

	NearestCastHitCollector collector(QueryFilter(this));
	shape->cast_shape(world, p_transform, p_motion, filter, nearest_cast_callback, &collector);

	if (!collector.hit) {
		*p_closest_safe = 1.0;
		*p_closest_unsafe = 1.0;
		return true;
	}

	CastHit hit = collector.nearest_hit;
	*p_closest_safe = hit.fraction;
	*p_closest_unsafe = hit.fraction;

	return true;
}

Dictionary Box2DPhysicsDirectSpaceState2D::cast_shape(const Ref<PhysicsShapeQueryParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(!space, {});

	PhysicsShapeQueryParameters2D *params = p_parameters.ptr();

	b2WorldId world = space->get_world_id();
	b2QueryFilter filter = make_filter(params->get_collision_mask());

	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(params->get_shape_rid());
	ERR_FAIL_COND_V(!shape, {});

	Vector2 motion = params->get_motion();
	Transform2D start = params->get_transform();

	NearestCastHitCollector collector(QueryFilter(this));
	shape->cast_shape(world, start, motion, filter, nearest_cast_callback, &collector);

	if (!collector.hit) {
		return {};
	}

	Dictionary result;
	CastHit hit = collector.nearest_hit;

	result["point"] = to_godot(hit.point);
	result["centroid"] = start.get_origin() + (hit.fraction * motion);
	result["normal"] = to_godot(hit.normal).normalized();
	result["shape"] = hit.shape->index;
	result["rid"] = hit.body->get_rid();
	ObjectID id = hit.body->get_instance_id();
	result["collider_id"] = id;
	if (id.is_valid()) {
		result["collider"] = get_instance_hack(id);
	}

	return result;
}