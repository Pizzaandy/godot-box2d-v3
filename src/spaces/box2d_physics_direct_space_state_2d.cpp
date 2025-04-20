
#include "box2d_physics_direct_space_state_2d.h"
#include "../bodies/box2d_body_2d.h"
#include "../box2d_physics_server_2d.h"
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

	overlap_results.clear();
	b2ShapeProxy proxy = b2MakeProxy(&transform.p, 1, 0.0);
	OverlapQueryCollector collector(p_max_results, query_filter, overlap_results);
	b2World_OverlapShape(space->get_world_id(), &proxy, box2d_filter, overlap_callback, &collector);

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

	return collector.count;
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
		overlap_results.clear();

		b2Vec2 from = to_box2d(p_from);
		b2ShapeProxy proxy = b2MakeProxy(&from, 1, 0.0);
		OverlapQueryCollector collector(1, query_filter, overlap_results);
		b2World_OverlapShape(space->get_world_id(), &proxy, box2d_filter, overlap_callback, &collector);

		if (collector.count > 0) {
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

	cast_results.clear();
	CastQueryCollector collector(1, query_filter, true, cast_results);
	b2World_CastRay(space->get_world_id(), to_box2d(p_from), to_box2d(p_to - p_from), box2d_filter, cast_callback, &collector);

	if (collector.count == 0) {
		return false;
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, 0);

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

	PhysicsServer2DExtensionRayResult &result = *p_result;

	result.position = hit.point;
	result.normal = hit.normal;
	result.shape = hit.shape->get_index();
	result.rid = hit.object->get_rid();
	result.collider_id = hit.object->get_instance_id();
	if (result.collider_id.is_valid()) {
		result.collider = get_instance_hack(result.collider_id);
	}

	return true;
}

/// A shape cast that returns overlapped shapes
/// Includes initial overlaps I think
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
	query.translation = p_motion;
	query.find_nearest = false;
	query.margin = p_margin;
	query.ignore_intial_overlaps = false;

	cast_results.clear();
	shape->cast(query, p_transform, cast_results);

	int count = 0;
	for (CastHit &hit : cast_results) {
		ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

		PhysicsServer2DExtensionShapeResult &result = *p_result++;
		result.shape = hit.shape->get_index();
		result.rid = hit.object->get_rid();
		result.collider_id = hit.object->get_instance_id();
		if (result.collider_id.is_valid()) {
			result.collider = get_instance_hack(result.collider_id);
		}

		count++;
	}

	return count;
}

/// A shape cast that checks how far a shape can move without colliding.
/// Ignores initial overlaps.
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
	query.translation = p_motion;
	query.find_nearest = true;
	query.margin = p_margin;
	query.ignore_intial_overlaps = true;

	cast_results.clear();
	int count = shape->cast(query, p_transform, cast_results);

	if (count == 0) {
		*p_closest_safe = 1.0;
		*p_closest_unsafe = 1.0;
		return true;
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, 0);

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, 0);

	*p_closest_safe = box2d_compute_safe_fraction(hit.fraction, p_motion.length());
	*p_closest_unsafe = hit.fraction;

	return true;
}

/// A shape cast that returns collision points.
/// Ignores initial overlaps in GodotPhysics but not mentioned in docs - they are enabled here.
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

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.translation = p_motion;
	query.find_nearest = false;
	query.margin = p_margin;
	query.ignore_intial_overlaps = false;

	cast_results.clear();
	int count = shape->cast(query, p_transform, cast_results);
	cast_results.sort();

	if (count == 0) {
		return false;
	}

	Vector2 *points = static_cast<Vector2 *>(p_results);

	int index = 0;
	for (int i = 0; i < count; i++) {
		points[index++] = cast_results[i].point + (cast_results[i].normal * p_margin);
		points[index++] = cast_results[i].point;
	}

	*p_result_count = index / 2;

	return true;
}

/// A shape cast that checks for initial overlaps. Returns the first collision found.
bool Box2DDirectSpaceState2D::_rest_info(
		const RID &p_shape_rid,
		const Transform2D &p_transform,
		const Vector2 &p_motion,
		float p_margin,
		uint32_t p_collision_mask,
		bool p_collide_with_bodies,
		bool p_collide_with_areas,
		PhysicsServer2DExtensionShapeRestInfo *p_rest_info) {
	ERR_FAIL_COND_V(!space, false);
	Box2DShape2D *shape = Box2DPhysicsServer2D::get_singleton()->get_shape(p_shape_rid);
	ERR_FAIL_COND_V(!shape, false);

	b2QueryFilter box2d_filter = make_filter(p_collision_mask, p_collide_with_bodies, p_collide_with_areas);
	SpaceStateQueryFilter query_filter(this, box2d_filter);

	OverlapQuery overlap_query;
	overlap_query.world = space->get_world_id();
	overlap_query.max_results = 1;
	overlap_query.filter = query_filter;
	overlap_query.margin = p_margin;

	overlap_results.clear();
	int overlap_count = shape->overlap(overlap_query, p_transform, overlap_results);

	if (overlap_count > 0) {
		ShapeOverlap overlap = overlap_results[0];

		ShapeCollideResult result = box2d_collide_shapes(
				overlap.source_shape,
				b2Transform_identity,
				overlap.shape_id,
				b2Body_GetTransform(overlap.object->get_body_id()));

		if (result.point_count == 0) {
			return false;
		}

		// Using the point with the highest depth results in flickering on certain surfaces.
		// Simply using the first collision point makes this more consistent with Godot Physics.
		ShapeCollidePoint point = result.points[0];

		p_rest_info->point = point.point;
		p_rest_info->normal = result.normal;
		p_rest_info->rid = overlap.object->get_rid();
		p_rest_info->collider_id = overlap.object->get_instance_id();
		p_rest_info->shape = overlap.shape->get_index();

		Box2DBody2D *body = overlap.object->as_body();
		if (body) {
			p_rest_info->linear_velocity = body->get_velocity_at_point(p_rest_info->point);
		} else {
			p_rest_info->linear_velocity = Vector2();
		}

		return true;
	}

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.translation = p_motion;
	query.find_nearest = true;
	query.margin = p_margin;

	cast_results.clear();
	int cast_count = shape->cast(query, p_transform, cast_results);

	if (cast_count > 0) {
		int index = find_nearest_cast_hit(cast_results);
		ERR_FAIL_COND_V(index == -1, 0);
		CastHit &hit = cast_results[index];

		p_rest_info->point = hit.point;
		p_rest_info->normal = hit.normal;
		p_rest_info->rid = hit.object->get_rid();
		p_rest_info->collider_id = hit.object->get_instance_id();
		p_rest_info->shape = hit.shape->get_index();

		Box2DBody2D *body = hit.object->as_body();
		if (body) {
			p_rest_info->linear_velocity = body->get_velocity_at_point(p_rest_info->point);
		} else {
			p_rest_info->linear_velocity = Vector2();
		}

		return true;
	}

	return false;
}

/// Custom shape cast that returns both collisions and travel distance.
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

	Transform2D transform = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = 1;
	query.filter = query_filter;
	query.translation = motion;
	query.find_nearest = true;
	query.margin = params->get_margin();

	cast_results.clear();
	int count = shape->cast(query, transform, cast_results);

	if (count == 0) {
		return {};
	}

	int index = find_nearest_cast_hit(cast_results);
	ERR_FAIL_COND_V(index == -1, {});

	CastHit &hit = cast_results[index];
	ERR_FAIL_COND_V(hit.shape->get_index() < 0, {});

	Dictionary result;
	result["point"] = hit.point;
	result["destination"] = transform.get_origin() + (hit.fraction * motion);
	result["normal"] = hit.normal;
	result["shape"] = hit.shape->get_index();
	result["rid"] = hit.object->get_rid();
	ObjectID id = hit.object->get_instance_id();
	result["collider_id"] = id;
	if (id.is_valid()) {
		result["collider"] = get_instance_hack(id);
	}

	return result;
}

/// Custom shape cast that returns both collisions and travel distance for multiple shapes.
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

	Transform2D transform = params->get_transform();
	Vector2 motion = params->get_motion();

	CastQuery query;
	query.world = space->get_world_id();
	query.max_results = p_max_results;
	query.filter = query_filter;
	query.translation = motion;
	query.find_nearest = false;
	query.margin = params->get_margin();

	cast_results.clear();
	int count = shape->cast(query, transform, cast_results);
	cast_results.sort();

	if (count == 0) {
		return {};
	}

	TypedArray<Dictionary> hits;

	for (CastHit &hit : cast_results) {
		Dictionary result;
		result["point"] = hit.point;
		result["destination"] = transform.get_origin() + (hit.fraction * motion);
		result["normal"] = hit.normal;
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