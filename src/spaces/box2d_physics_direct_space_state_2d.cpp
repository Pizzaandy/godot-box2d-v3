
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../servers/box2d_physics_server_2d.h"
#include "box2d_query.h"

#include <godot_cpp/classes/physics_shape_query_parameters2d.hpp>

static thread_local LocalVector<CastHit> cast_results;
static thread_local LocalVector<ShapeOverlap> overlap_results;

void Box2DDirectSpaceState2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("cast_shape"), &Box2DDirectSpaceState2D::cast_shape, "parameters");
	ClassDB::bind_method(D_METHOD("cast_shape_all"), &Box2DDirectSpaceState2D::cast_shape_all, "parameters", "max_results");
}

b2QueryFilter Box2DDirectSpaceState2D::make_filter(uint64_t p_collision_mask, bool p_collide_bodies, bool p_collide_areas) {
	b2QueryFilter result = b2QueryFilter{ 0, p_collision_mask };
	if (p_collide_bodies) {
		result.categoryBits |= BODY_MASK_BIT;
	}
	if (p_collide_areas) {
		result.categoryBits |= AREA_MASK_BIT;
	}
	return result;
}

int32_t Box2DDirectSpaceState2D::_intersect_point(
		const Vector2 &p_position,
		uint64_t p_canvas_instance_id,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeResult *p_results,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, 0);

	if (p_max_results <= 0) {
		return 0;
	}

	b2Transform transform = b2Transform_identity;
	transform.p = to_box2d(p_position);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	OverlapQueryCollector collector(p_max_results, query_filter, overlap_results);
	b2World_OverlapPoint(space->get_world_id(), to_box2d(Vector2()), transform, box2d_filter, overlap_callback, &collector);

	for (ShapeOverlap overlap : collector.results) {
		ERR_FAIL_COND_V(overlap.shape->get_index() < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_results++;

		result.shape = overlap.shape->get_index();
		result.rid = overlap.object->get_rid();
		result.collider_id = overlap.object->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return collector.results.size();
}

bool Box2DDirectSpaceState2D::_intersect_ray(
		const Vector2 &p_from,
		const Vector2 &p_to,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		bool p_hit_from_inside,
		PhysicsServer2DExtensionRayResult *p_result) {
	ERR_FAIL_COND_V(!space, 0);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	if (p_hit_from_inside) {
		OverlapQueryCollector collector(1, query_filter, overlap_results);

		b2World_OverlapPoint(space->get_world_id(), to_box2d(p_from), b2Transform_identity, box2d_filter, overlap_callback, &collector);

		if (collector.results.size() > 0) {
			ShapeOverlap overlap = collector.results[0];
			ERR_FAIL_COND_V(overlap.shape->get_index() < 0, false);

			p_result->position = p_from;
			p_result->normal = Vector2();
			p_result->shape = overlap.shape->get_index();
			p_result->rid = overlap.object->get_rid();
			p_result->collider_id = overlap.object->get_instance_id();
			if (p_result->collider_id.is_valid()) {
				p_result->collider = get_instance_hack(p_result->collider_id);
			}
			return true;
		}
	}

	CastQueryCollector collector(1, query_filter, cast_results, true);
	b2World_CastRay(space->get_world_id(), to_box2d(p_from), to_box2d(p_to - p_from), box2d_filter, cast_callback, &collector);

	if (collector.results.size() == 0) {
		return false;
	}

	CastHit hit = collector.results[0];

	ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

	PhysicsServer2DExtensionRayResult &result = *p_result;

	result.position = to_godot(hit.point);
	result.normal = to_godot(hit.normal).normalized();
	result.shape = hit.shape->get_index();
	result.rid = hit.object->get_rid();
	result.collider_id = hit.object->get_instance_id();
	if (result.collider_id.is_valid()) {
		result.collider = get_instance_hack(result.collider_id);
	}

	return true;
}

int32_t Box2DDirectSpaceState2D::_intersect_shape(
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
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, 0);

	if (p_max_results <= 0) {
		return 0;
	}

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.origin = p_transform;
	query.translation = p_motion;
	query.find_nearest = false;

	shape->cast(query, cast_results);

	for (CastHit hit : cast_results) {
		ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_result++;

		result.shape = hit.shape->get_index();
		result.rid = hit.object->get_rid();
		result.collider_id = hit.object->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}
	}

	return cast_results.size();
}

bool Box2DDirectSpaceState2D::_cast_motion(
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
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_NULL_V(shape, false);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.origin = p_transform;
	query.translation = p_motion;
	query.find_nearest = true;

	int hits = shape->cast(query, cast_results);

	if (hits == 0) {
		*p_closest_safe = 1.0;
		*p_closest_unsafe = 1.0;
		return true;
	}

	CastHit hit = cast_results[0];

	const float adjustment = 0.1f;

	float distance = hit.fraction * p_motion.length();
	float adjusted_distance = MAX(0.0f, distance - adjustment);

	*p_closest_safe = adjusted_distance / p_motion.length();
	*p_closest_unsafe = hit.fraction;

	return true;
}

bool Box2DDirectSpaceState2D::_collide_shape(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		void *p_results,
		int32_t p_max_results,
		int32_t *p_result_count) {
	ERR_FAIL_COND_V(!space, false);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, false);

	if (p_max_results <= 0) {
		return 0;
	}

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	OverlapQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.origin = p_transform;

	shape->overlap(query, overlap_results);

	if (overlap_results.size() == 0) {
		return false;
	}

	Vector2 *points = static_cast<Vector2 *>(p_results);
	int max_points = p_max_results * 2;
	int point_count = 0;

	return false;

	// for (ShapeOverlap hit : results) {
	// 	points[point_count++] = to_godot(hit.point);
	// 	points[point_count++] = to_godot(b2Shape_GetClosestPoint(hit.shape_id, hit.point));

	// 	if (point_count >= max_points) {
	// 		break;
	// 	}
	// }

	// *p_result_count = point_count / 2;

	// return true;
}

bool Box2DDirectSpaceState2D::_rest_info(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeRestInfo *p_rest_info) {
	return false;

	// ERR_FAIL_COND_V(!space, false);

	// b2WorldId world = space->get_world_id();
	// b2QueryFilter filter = make_filter(p_collision_mask);

	// Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	// ERR_FAIL_COND_V(!shape, false);

	// ShapeGeometry shape_geometry = shape->get_shape_geometry(p_transform);

	// SpaceStateQueryFilter query_filter(this);
	// query_filter.collide_with_areas = p_collide_with_areas;
	// query_filter.collide_with_bodies = p_collide_with_bodies;
	// ShapeOverlapCollector collector(8, query_filter);
	// box2d_overlap_shape(world, shape_geometry, b2Transform_identity, filter, overlap_callback, &collector);

	// if (collector.overlaps.size() == 0) {
	// 	return false;
	// }

	// for (ShapeOverlap overlap : collector.overlaps) {
	// 	ShapeCollideResult result = box2d_collide_shapes(
	// 			shape_geometry,
	// 			b2Transform_identity,
	// 			get_shape_geometry(overlap.shape_id),
	// 			to_box2d(overlap.object->get_transform()),
	// 			false);

	// 	if (result.point_count == 0) {
	// 		continue;
	// 	}

	// 	p_rest_info->point = result.points[0].point;
	// 	p_rest_info->normal = result.normal;
	// 	p_rest_info->rid = overlap.object->get_rid();
	// 	p_rest_info->collider_id = overlap.object->get_instance_id();
	// 	p_rest_info->shape = overlap.shape->get_index();

	// 	Box2DBody2D *body = overlap.object->as_body();
	// 	if (body) {
	// 		p_rest_info->linear_velocity = body->get_velocity_at_local_point(result.points[0].point);
	// 	} else {
	// 		p_rest_info->linear_velocity = Vector2();
	// 	}

	// 	return true;
	// }

	// return false;
}

Dictionary Box2DDirectSpaceState2D::cast_shape(const Ref<PhysicsShapeQueryParameters2D> &p_parameters) {
	ERR_FAIL_COND_V(!space, {});
	PhysicsShapeQueryParameters2D *params = p_parameters.ptr();
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(params->get_shape_rid());
	ERR_FAIL_COND_V(!shape, {});

	b2QueryFilter box2d_filter = make_filter(
			params->get_collision_mask(),
			params->is_collide_with_bodies_enabled(),
			params->is_collide_with_areas_enabled());

	ArrayQueryFilter query_filter(params->get_exclude(), box2d_filter);

	Transform2D origin = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.origin = origin;
	query.translation = motion;
	query.find_nearest = true;

	shape->cast(query, cast_results);

	if (cast_results.size() == 0) {
		return {};
	}

	CastHit hit = cast_results[0];

	Dictionary result;
	result["point"] = to_godot(hit.point);
	result["destination"] = origin.get_origin() + (hit.fraction * motion);
	result["normal"] = to_godot(hit.normal).normalized();
	result["shape"] = hit.shape->get_index();
	result["rid"] = hit.object->get_rid();
	ObjectID id = hit.object->get_instance_id();
	result["collider_id"] = id;
	if (id.is_valid()) {
		result["collider"] = get_instance_hack(id);
	}

	return result;
}

TypedArray<Dictionary> Box2DDirectSpaceState2D::cast_shape_all(
		const Ref<PhysicsShapeQueryParameters2D> &p_parameters,
		int32_t p_max_results) {
	ERR_FAIL_COND_V(!space, {});
	PhysicsShapeQueryParameters2D *params = p_parameters.ptr();
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(params->get_shape_rid());
	ERR_FAIL_COND_V(!shape, {});

	if (p_max_results <= 0) {
		return {};
	}

	b2QueryFilter box2d_filter = make_filter(
			params->get_collision_mask(),
			params->is_collide_with_bodies_enabled(),
			params->is_collide_with_areas_enabled());

	ArrayQueryFilter query_filter(params->get_exclude(), box2d_filter);

	Transform2D origin = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.origin = origin;
	query.translation = motion;
	query.find_nearest = false;

	shape->cast(query, cast_results);

	if (cast_results.size() == 0) {
		return {};
	}

	TypedArray<Dictionary> hits;

	for (CastHit hit : cast_results) {
		Dictionary result;
		result["point"] = to_godot(hit.point);
		result["destination"] = origin.get_origin() + (hit.fraction * motion);
		result["normal"] = to_godot(hit.normal).normalized();
		result["shape"] = hit.shape->get_index();
		result["rid"] = hit.object->get_rid();
		ObjectID id = hit.object->get_instance_id();
		result["collider_id"] = id;
		if (id.is_valid()) {
			result["collider"] = get_instance_hack(id);
		}
		hits.push_back(result);
	}

	return hits;
}